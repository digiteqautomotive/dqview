#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "streams.h"

DEFINE_GUID(CLSID_FrameBuffer, 0xfd501041, 0x8ebe, 0x11ce, 0x81, 0x83, 0x00,
  0xaa, 0x00, 0x57, 0x7d, 0xa1);

class FrameBufferStream;

class FrameBuffer: public CSource
{
public:
	DECLARE_IUNKNOWN;

	FrameBuffer(int iWidth, int iHeight, const char *pBuffer, HANDLE hMutex,
	  HRESULT *phr);
	~FrameBuffer();

	// IMediaFilter
	STDMETHODIMP Run(REFERENCE_TIME tStart);
	STDMETHODIMP Pause(void);
	STDMETHODIMP Stop(void);

private:
	FrameBufferStream *m_pStream;
};

#endif // FRAMEBUFFER_H
