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
	if (!pFrame)
		return S_OK;
	if (pFrame->TimeStamp() > clock + 1000)
		Sleep((pFrame->TimeStamp() - clock) / 1000);

	if ((LONG)pFrame->size() != pMediaSample->GetSize())
		return E_INVALIDARG;

	if (IsEqualGUID(pFilter->Format(), MEDIASUBTYPE_RGB32)) {
		// verticaly flip the image
		for (int i = 0; i < pFilter->Height(); i++)
			memcpy(pData + (i * pFilter->Width() * 4), pFrame->Buffer()
			  + ((pFilter->Height() - 1 - i) * pFilter->Width() * 4),
			  pFilter->Width() * 4);
	} else
		memcpy(pData, pFrame->Buffer(), pFilter->Width() * pFilter->Height() * 2);

	pFilter->FrameQueue().pop();

	return S_OK;
}

HRESULT FrameBufferStream::GetMediaType(CMediaType *pMediaType)
{
	CheckPointer(pMediaType, E_POINTER);

	FrameBuffer *pFilter = static_cast<FrameBuffer*>(m_pFilter);
	CAutoLock cAutoLock(m_pFilter->pStateLock());

	VIDEOINFO *pVI = (VIDEOINFO *)pMediaType->AllocFormatBuffer(sizeof(VIDEOINFO));
	if (!pVI)
		return(E_OUTOFMEMORY);

	ZeroMemory(pVI, sizeof(VIDEOINFO));
	if (IsEqualGUID(pFilter->Format(), MEDIASUBTYPE_RGB32)) {
		pVI->bmiHeader.biCompression = BI_RGB;
		pVI->bmiHeader.biBitCount = 32;
	} else {
		pVI->bmiHeader.biCompression = mmioFOURCC('Y', 'U', 'Y', '2');
		pVI->bmiHeader.biBitCount = 16;
	}
	pVI->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pVI->bmiHeader.biWidth = pFilter->Width();
	pVI->bmiHeader.biHeight = pFilter->Height();
	pVI->bmiHeader.biPlanes = 1;
	pVI->bmiHeader.biSizeImage = GetBitmapSize(&pVI->bmiHeader);
	pVI->bmiHeader.biClrImportant = 0;
	pVI->AvgTimePerFrame = pFilter->TimePerFrame();
	SetRectEmpty(&(pVI->rcSource));
	SetRectEmpty(&(pVI->rcTarget));

	pMediaType->SetType(&MEDIATYPE_Video);
	pMediaType->SetSubtype(&(pFilter->Format()));
	pMediaType->SetFormatType(&FORMAT_VideoInfo);
	pMediaType->SetTemporalCompression(FALSE);

	const GUID SubTypeGUID = GetBitmapSubtype(&pVI->bmiHeader);
	pMediaType->SetSubtype(&SubTypeGUID);
	pMediaType->SetSampleSize(pVI->bmiHeader.biSizeImage);

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

FrameBuffer::FrameBuffer(AM_MEDIA_TYPE *pMT, int iCapacity, HRESULT *phr)
  : CSource(NAME("Frame Buffer"), NULL, CLSID_FrameBuffer),
    m_pQueue(iCapacity, (((VIDEOINFO*)(pMT->pbFormat))->AvgTimePerFrame * 10)
      / 10000)
{
	CAutoLock cAutoLock(&m_cStateLock);

	m_Format = pMT->formattype;

	VIDEOINFO *vi = reinterpret_cast<VIDEOINFO*>(pMT->pbFormat);
	m_iWidth = vi->bmiHeader.biWidth;
	m_iHeight = vi->bmiHeader.biHeight;
	m_iTimePerFrame = vi->AvgTimePerFrame;

	m_pStream = new FrameBufferStream(this, phr);
	m_pStream->AddRef();
}

FrameBuffer::~FrameBuffer()
{
	CAutoLock cAutoLock(&m_cStateLock);

	m_pStream->Release();
}
