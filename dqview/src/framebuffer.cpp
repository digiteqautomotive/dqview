#include <olectl.h>
#include <initguid.h>
#include <vlc/vlc.h>
#include "framebuffer.h"

const REGPINTYPES MediaTypes[1] = {
	{&MEDIATYPE_Video, &MEDIASUBTYPE_RGB32},
};

class FrameBufferStream : public CSourceStream, public IAMStreamConfig
{
public:
	DECLARE_IUNKNOWN;

	FrameBufferStream(FrameBuffer::Queue *pQueue, FrameBuffer *pParent,
	  HRESULT *phr);

	// CSourceStream
	HRESULT FillBuffer(IMediaSample *pMediaSample);
	HRESULT DecideBufferSize(IMemAllocator *pIMemAlloc,
	  ALLOCATOR_PROPERTIES *pProperties);
	HRESULT SetMediaType(const CMediaType *pMediaType);
	HRESULT CheckMediaType(const CMediaType *pMediaType);
	HRESULT GetMediaType(int iPosition, CMediaType *pmt);

	// IAMStreamConfig
	STDMETHODIMP GetFormat(AM_MEDIA_TYPE **ppmt);
	STDMETHODIMP SetFormat(AM_MEDIA_TYPE *pmt);
	STDMETHODIMP GetNumberOfCapabilities(int *piCount, int *piSize);
	STDMETHODIMP GetStreamCaps(int iIndex, AM_MEDIA_TYPE **ppmt, BYTE *pSCC);

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

HRESULT FrameBufferStream::GetFormat(AM_MEDIA_TYPE **ppmt)
{
	CheckPointer(ppmt, E_POINTER);
	const CMediaType *pCurrentMt = &m_mt;

	*ppmt = (AM_MEDIA_TYPE*)CoTaskMemAlloc(sizeof(AM_MEDIA_TYPE));
	memcpy(*ppmt, pCurrentMt, sizeof(AM_MEDIA_TYPE));
	if ((*ppmt)->pbFormat != NULL && (*ppmt)->cbFormat > 0) {
		(*ppmt)->pbFormat = (BYTE*)CoTaskMemAlloc(pCurrentMt->cbFormat);
		memcpy((*ppmt)->pbFormat, pCurrentMt->pbFormat, pCurrentMt->cbFormat);
	}

	return S_OK;
}

HRESULT FrameBufferStream::SetFormat(AM_MEDIA_TYPE *pmt)
{
	CheckPointer(pmt, E_POINTER);

	for (size_t i = 0; i < sizeof(MediaTypes) / sizeof(REGPINTYPES); i++) {
		if (*MediaTypes[i].clsMajorType == pmt->majortype
		  && *MediaTypes[i].clsMinorType == pmt->subtype) {
			CMediaType mt;
			HRESULT hr = GetMediaType(i,&mt);

			if (FAILED(hr))
				return hr;
			mt = *pmt;
			return SetMediaType(&mt);
		}
	}

	return E_INVALIDARG;
}

HRESULT FrameBufferStream::GetNumberOfCapabilities(int *piCount, int *piSize)
{
	CheckPointer(piCount, E_POINTER);
	CheckPointer(piSize, E_POINTER);

	*piCount = sizeof(MediaTypes) / sizeof(REGPINTYPES);
	*piSize = sizeof(VIDEO_STREAM_CONFIG_CAPS);

	return S_OK;
}

HRESULT FrameBufferStream::GetStreamCaps(int iIndex, AM_MEDIA_TYPE **ppmt,
  BYTE *pSCC)
{
	CheckPointer(ppmt, E_POINTER);

	CMediaType mt;
	HRESULT hr = GetMediaType(iIndex, &mt);
	if (FAILED(hr))
		return hr;

	*ppmt = (AM_MEDIA_TYPE*)CoTaskMemAlloc(sizeof(AM_MEDIA_TYPE));
	memcpy(*ppmt, (AM_MEDIA_TYPE*)&mt, sizeof(AM_MEDIA_TYPE));

	if ((*ppmt)->pbFormat!=NULL && (*ppmt)->cbFormat>=sizeof(VIDEOINFOHEADER)) {
		(*ppmt)->pbFormat = (BYTE*)CoTaskMemAlloc(mt.cbFormat);
		memcpy((*ppmt)->pbFormat, mt.pbFormat, mt.cbFormat);

		VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)(*ppmt)->pbFormat;
		if (pvi->AvgTimePerFrame==0)
			pvi->AvgTimePerFrame = 40 * 10000;	// 60Hz in 100ns intervals.
		if (pvi->dwBitRate==0)
			pvi->dwBitRate = pvi->bmiHeader.biSizeImage * 8 * 60;
	}

	if (pSCC) {
		VIDEO_STREAM_CONFIG_CAPS *pScc = (VIDEO_STREAM_CONFIG_CAPS *)pSCC;

		pScc->guid = mt.formattype;
		pScc->VideoStandard = 0;
		pScc->InputSize.cx = m_pQueue->Width();
		pScc->MaxCroppingSize.cx = m_pQueue->Width();
		pScc->MinCroppingSize.cx = m_pQueue->Width();
		pScc->InputSize.cy = m_pQueue->Height();
		pScc->MaxCroppingSize.cy = m_pQueue->Height();
		pScc->MinCroppingSize.cy = m_pQueue->Height();
		pScc->CropGranularityX = 1;
		pScc->CropGranularityY = 1;
		pScc->CropAlignX = 1;
		pScc->CropAlignY = 1;
		pScc->MaxOutputSize.cx = m_pQueue->Width();
		pScc->MinOutputSize.cx = m_pQueue->Width();
		pScc->MaxOutputSize.cy = m_pQueue->Height();
		pScc->MinOutputSize.cy = m_pQueue->Height();
		pScc->OutputGranularityX = 1;
		pScc->OutputGranularityY = 1;
		pScc->StretchTapsX = 0;
		pScc->StretchTapsY = 0;
		pScc->ShrinkTapsX = 0;
		pScc->ShrinkTapsY = 0;
		pScc->MaxFrameInterval = 10000000;		// 1Hz
		pScc->MinFrameInterval = 10000000 / 60;	// 60Hz
		pScc->MinBitsPerSecond = m_pQueue->Width() * m_pQueue->Height() * 4 * 8;
		pScc->MaxBitsPerSecond = pScc->MinBitsPerSecond * 60;
	}

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

HRESULT FrameBuffer::Run(REFERENCE_TIME tStart)
{
	m_pStream->Run();
	return CBaseFilter::Run(tStart);
}

HRESULT FrameBuffer::Pause(void)
{
	return CBaseFilter::Pause();
}

HRESULT FrameBuffer::Stop(void)
{
	m_pStream->Stop();
	return CBaseFilter::Stop();
}
