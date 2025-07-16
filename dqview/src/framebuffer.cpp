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
	HRESULT GetMediaType(CMediaType *pMediaType);

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
	if (m_pQueue->Width() * m_pQueue->Height() * 4 != pMediaSample->GetSize())
		return E_INVALIDARG;
	HRESULT hr = pMediaSample->GetPointer(&pData);
	if (FAILED(hr))
		return hr;

	m_pQueue->Lock();
	if (m_pQueue->IsEmpty()) {
		m_pQueue->Unlock();
		return S_OK;
	}

	FrameBuffer::Queue::Frame *pFrame = m_pQueue->Read();
	// verticaly flip the image
	for (int i = 0; i < m_pQueue->Height(); i++)
		memcpy(pData + (i * m_pQueue->Width() * 4), pFrame->Buffer()
		  + ((m_pQueue->Height() - 1 - i) * m_pQueue->Width() * 4),
		  m_pQueue->Width() * 4);

	pMediaSample->SetMediaTime(0, 0);

	if (pFrame->TimeStamp() <= libvlc_clock())
		m_pQueue->Pop();

	m_pQueue->Unlock();

	return S_OK;
}

HRESULT FrameBufferStream::GetMediaType(CMediaType *pMediaType)
{
	CheckPointer(pMediaType, E_POINTER);

	CAutoLock cAutoLock(m_pFilter->pStateLock());

	VIDEOINFO *pvi = (VIDEOINFO *)pMediaType->AllocFormatBuffer(sizeof(VIDEOINFO));
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
