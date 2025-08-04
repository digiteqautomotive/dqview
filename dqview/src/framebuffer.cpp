#include <olectl.h>
#include <initguid.h>
#include <vlc/vlc.h>
#include "framebuffer.h"

class FrameBufferStream : public CSourceStream
{
public:
	DECLARE_IUNKNOWN;

	FrameBufferStream(FrameBuffer *pParent, HRESULT *phr);

	HRESULT FillBuffer(IMediaSample *pMediaSample);
	HRESULT DecideBufferSize(IMemAllocator *pIMemAlloc,
	  ALLOCATOR_PROPERTIES *pProperties);
	HRESULT GetMediaType(CMediaType *pMediaType);
};

FrameBufferStream::FrameBufferStream(FrameBuffer *pParent, HRESULT *phr)
  : CSourceStream(NAME("Stream"), phr, pParent, NAME("Pin"))
{
}

HRESULT FrameBufferStream::FillBuffer(IMediaSample *pMediaSample)
{
	FrameBuffer *pFilter = static_cast<FrameBuffer*>(m_pFilter);
	int64_t clock(libvlc_clock());
	BYTE *pData;

	HRESULT hr = pMediaSample->GetPointer(&pData);
	if (FAILED(hr))
		return hr;

	FrameBuffer::Frame *pFrame = pFilter->FrameQueue().top();
	while (pFrame->TimeStamp() < clock) {
		pFilter->FrameQueue().pop();
		pFrame = pFilter->FrameQueue().top();
	}

	if ((LONG)pFrame->size() != pMediaSample->GetSize())
		return E_INVALIDARG;

	if (pFilter->Format() == RGB) {
		// verticaly flip the image
		for (int i = 0; i < pFilter->Height(); i++)
			memcpy(pData + (i * pFilter->Width() * 4), pFrame->Buffer()
			  + ((pFilter->Height() - 1 - i) * pFilter->Width() * 4),
			  pFilter->Width() * 4);
	} else
		memcpy(pData, pFrame->Buffer(), pFilter->Width() * pFilter->Height() * 2);

	return S_OK;
}

HRESULT FrameBufferStream::GetMediaType(CMediaType *pMediaType)
{
	CheckPointer(pMediaType, E_POINTER);

	FrameBuffer *pFilter = static_cast<FrameBuffer*>(m_pFilter);
	CAutoLock cAutoLock(m_pFilter->pStateLock());

	VIDEOINFO *pvi = (VIDEOINFO *)pMediaType->AllocFormatBuffer(sizeof(VIDEOINFO));
	if (!pvi)
		return(E_OUTOFMEMORY);

	ZeroMemory(pvi, sizeof(VIDEOINFO));
	if (pFilter->Format() == RGB) {
		pvi->bmiHeader.biCompression = BI_RGB;
		pvi->bmiHeader.biBitCount = 32;
	} else {
		pvi->bmiHeader.biCompression = mmioFOURCC('Y', 'U', 'Y', '2');
		pvi->bmiHeader.biBitCount = 16;
	}
	pvi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pvi->bmiHeader.biWidth = pFilter->Width();
	pvi->bmiHeader.biHeight = pFilter->Height();
	pvi->bmiHeader.biPlanes = 1;
	pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);
	pvi->bmiHeader.biClrImportant = 0;
	pvi->AvgTimePerFrame = pFilter->TimePerFrame();
	SetRectEmpty(&(pvi->rcSource));
	SetRectEmpty(&(pvi->rcTarget));

	pMediaType->SetType(&MEDIATYPE_Video);
	pMediaType->SetFormatType(&FORMAT_VideoInfo);
	pMediaType->SetTemporalCompression(FALSE);

	const GUID SubTypeGUID = GetBitmapSubtype(&pvi->bmiHeader);
	pMediaType->SetSubtype(&SubTypeGUID);
	pMediaType->SetSampleSize(pvi->bmiHeader.biSizeImage);

	return S_OK;
}

HRESULT FrameBufferStream::DecideBufferSize(IMemAllocator *pAlloc,
  ALLOCATOR_PROPERTIES *pProperties)
{
	CheckPointer(pAlloc, E_POINTER);
	CheckPointer(pProperties, E_POINTER);

	CAutoLock cAutoLock(m_pFilter->pStateLock());
	HRESULT hr;

	const VIDEOINFO *pvi = (VIDEOINFO *)m_mt.Format();
	pProperties->cBuffers = 1;
	pProperties->cbBuffer = pvi->bmiHeader.biSizeImage;

	ALLOCATOR_PROPERTIES Actual;
	hr = pAlloc->SetProperties(pProperties, &Actual);
	if (FAILED(hr))
		return hr;
	if (Actual.cbBuffer < pProperties->cbBuffer)
		return E_FAIL;

	return S_OK;
}

FrameBuffer::FrameBuffer(PixelFormat Format, int iWidth, int iHeight, int iTPF,
  int iCapacity, HRESULT *phr)
  : CSource(NAME("Frame Buffer"), NULL, CLSID_FrameBuffer), m_iWidth(iWidth),
    m_iHeight(iHeight), m_iTimePerFrame(iTPF), m_Format(Format),
    m_pQueue(iCapacity)
{
	CAutoLock cAutoLock(&m_cStateLock);

	m_pStream = new FrameBufferStream(this, phr);
	m_pStream->AddRef();
}

FrameBuffer::~FrameBuffer()
{
	m_pStream->Release();
}
