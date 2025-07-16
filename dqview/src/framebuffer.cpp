#include <olectl.h>
#include <initguid.h>
#include <vlc/vlc.h>
#include "framebuffer.h"

class FrameBufferStream : public CSourceStream
{
public:
	DECLARE_IUNKNOWN;

	FrameBufferStream(FrameBuffer::Queue *pQueue, FrameBuffer *pParent,
	  HRESULT *phr);

	HRESULT FillBuffer(IMediaSample *pMediaSample);
	HRESULT DecideBufferSize(IMemAllocator *pIMemAlloc,
	  ALLOCATOR_PROPERTIES *pProperties);
	HRESULT SetMediaType(const CMediaType *pMediaType);
	HRESULT CheckMediaType(const CMediaType *pMediaType);
	HRESULT GetMediaType(int iPosition, CMediaType *pmt);

private:
	FrameBuffer::Queue *m_pQueue;
};

FrameBufferStream::FrameBufferStream(FrameBuffer::Queue *pQueue,
  FrameBuffer *pParent, HRESULT *phr)
  : CSourceStream(NAME("Stream"), phr, pParent, NAME("Pin")), m_pQueue(pQueue)
{
}

HRESULT FrameBufferStream::FillBuffer(IMediaSample *pMediaSample)
{
	BYTE *pData;
	long lDataLen = pMediaSample->GetSize();
	if (m_pQueue->Width() * m_pQueue->Height() * 4 != lDataLen)
		return E_INVALIDARG;
	HRESULT hr = pMediaSample->GetPointer(&pData);
	if (FAILED(hr))
		return hr;

	while (true) {
		m_pQueue->Lock();
		if (m_pQueue->IsEmpty()) {
			m_pQueue->Unlock();
			Sleep(10);
		} else
			break;
	};

	FrameBuffer::Queue::Frame *pFrame = m_pQueue->Read();
	// verticaly flip the image
	for (int i = 0; i < m_pQueue->Height(); i++)
		memcpy(pData + (i * m_pQueue->Width() * 4), pFrame->Buffer()
		  + ((m_pQueue->Height() - 1 - i) * m_pQueue->Width() * 4),
		  m_pQueue->Width() * 4);

	if (pFrame->TimeStamp() <= libvlc_clock())
		m_pQueue->Pop();

	m_pQueue->Unlock();

	return S_OK;
}

HRESULT FrameBufferStream::GetMediaType(int iPosition, CMediaType *pmt)
{
	CheckPointer(pmt, E_POINTER);

	if (iPosition < 0)
		return E_INVALIDARG;
	if (iPosition > 0)
		return VFW_S_NO_MORE_ITEMS;

	CAutoLock cAutoLock(m_pFilter->pStateLock());

	VIDEOINFO *pvi = (VIDEOINFO *)pmt->AllocFormatBuffer(sizeof(VIDEOINFO));
	if (!pvi)
		return(E_OUTOFMEMORY);

	ZeroMemory(pvi, sizeof(VIDEOINFO));
	pvi->bmiHeader.biCompression = BI_RGB;
	pvi->bmiHeader.biBitCount = 32;
	pvi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pvi->bmiHeader.biWidth = m_pQueue->Width();
	pvi->bmiHeader.biHeight = m_pQueue->Height();
	pvi->bmiHeader.biPlanes = 1;
	pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);
	pvi->bmiHeader.biClrImportant = 0;
	SetRectEmpty(&(pvi->rcSource));
	SetRectEmpty(&(pvi->rcTarget));

	pmt->SetType(&MEDIATYPE_Video);
	pmt->SetFormatType(&FORMAT_VideoInfo);
	pmt->SetTemporalCompression(FALSE);

	const GUID SubTypeGUID = GetBitmapSubtype(&pvi->bmiHeader);
	pmt->SetSubtype(&SubTypeGUID);
	pmt->SetSampleSize(pvi->bmiHeader.biSizeImage);

	return S_OK;
}

HRESULT FrameBufferStream::CheckMediaType(const CMediaType *pMediaType)
{
	CheckPointer(pMediaType, E_POINTER);

	if ((*(pMediaType->Type()) != MEDIATYPE_Video)
	  || !(pMediaType->IsFixedSize()))
		return E_INVALIDARG;

	const GUID *pSubType = pMediaType->Subtype();
	if (pSubType == NULL)
		return E_INVALIDARG;
	if (*pSubType != MEDIASUBTYPE_RGB32)
		return E_INVALIDARG;

	VIDEOINFO *pFormat = (VIDEOINFO *) pMediaType->Format();
	if (pFormat == NULL)
		return E_INVALIDARG;

	if (pFormat->bmiHeader.biWidth != m_pQueue->Width()
	  || abs(pFormat->bmiHeader.biHeight) != m_pQueue->Height())
		return E_INVALIDARG;

	return S_OK;
}

HRESULT FrameBufferStream::DecideBufferSize(IMemAllocator *pAlloc,
  ALLOCATOR_PROPERTIES *pProperties)
{
	CheckPointer(pAlloc, E_POINTER);
	CheckPointer(pProperties, E_POINTER);

	CAutoLock cAutoLock(m_pFilter->pStateLock());
	HRESULT hr;

	VIDEOINFO *pvi = (VIDEOINFO *)m_mt.Format();
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

HRESULT FrameBufferStream::SetMediaType(const CMediaType *pMediaType)
{
	CAutoLock cAutoLock(m_pFilter->pStateLock());

	HRESULT hr = CSourceStream::SetMediaType(pMediaType);
	if (SUCCEEDED(hr)) {
		VIDEOINFO *pvi = (VIDEOINFO *)m_mt.Format();
		return (!pvi || pvi->bmiHeader.biBitCount != 32)
		  ? E_UNEXPECTED : S_OK;
	}

	return hr;
}

FrameBuffer::FrameBuffer(int iWidth, int iHeight, int iCapacity, HRESULT *phr)
  : CSource(NAME("Frame Buffer"), NULL, CLSID_FrameBuffer)
{
	CAutoLock cAutoLock(&m_cStateLock);

	m_pQueue = new Queue(iWidth, iHeight, iCapacity);
	m_pStream = new FrameBufferStream(m_pQueue, this, phr);
	m_pStream->AddRef();

	if (SUCCEEDED(*phr))
		*phr = AddPin(m_pStream);
}

FrameBuffer::~FrameBuffer()
{
	m_pStream->Release();
	delete m_pQueue;
}
