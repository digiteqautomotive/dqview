#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <cstdint>
#include "streams.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

DEFINE_GUID(CLSID_FrameBuffer, 0xfd501041, 0x8ebe, 0x11ce, 0x81, 0x83, 0x00,
  0xaa, 0x00, 0x57, 0x7d, 0xa1);

class FrameBufferStream;

class FrameBuffer: public CSource
{
public:
	DECLARE_IUNKNOWN;

	struct Queue
	{
	public:
		struct Frame
		{
		public:
			Frame(int iWidth, int iHeight) : m_ts(0)
			{
				m_pBuffer = new char[iWidth * iHeight * 4];
			}
			~Frame() {delete[] m_pBuffer;}

			int64_t TimeStamp() const {return m_ts;}
			void SetTimeStamp(int64_t ts) {m_ts = ts;}
			char *Buffer() {return m_pBuffer;}

		private:
			int64_t m_ts;
			char *m_pBuffer;
		};

		Queue(int iWidth, int iHeight)
		  : m_iWidth(iWidth), m_iHeight(iHeight), m_iPush(0), m_iPop(0),
		  m_iSize(0)
		{
			for (size_t i = 0; i < ARRAY_SIZE(m_pData); i++)
				m_pData[i] = new Frame(iWidth, iHeight);
			m_hMutex = CreateMutex(NULL, FALSE, NULL);
		}
		~Queue()
		{
			for (size_t i = 0; i < ARRAY_SIZE(m_pData); i++)
				delete[] m_pData[i];
			CloseHandle(m_hMutex);
		}

		int Width() const {return m_iWidth;}
		int Height() const {return m_iHeight;}
		Frame *Read() {return m_pData[m_iPop];}
		void Pop()
		  {m_iPop = (m_iPop + 1) & (ARRAY_SIZE(m_pData) - 1); m_iSize--;}
		Frame *Write() {return m_pData[m_iPush];}
		void Push()
		  {m_iPush = (m_iPush + 1) & (ARRAY_SIZE(m_pData) - 1); m_iSize++;}
		void Lock() {WaitForSingleObject(m_hMutex, INFINITE);}
		void Unlock() {ReleaseMutex(m_hMutex);}
		bool IsEmpty() const {return m_iSize == 0;}
		bool IsFull() const {return m_iSize == ARRAY_SIZE(m_pData);}
		int Size() const {return m_iSize;}

	private:
		int m_iWidth, m_iHeight;
		Frame *m_pData[32];
		int m_iPush, m_iPop;
		int m_iSize;
		HANDLE m_hMutex;
	};

	FrameBuffer(int iWidth, int iHeight, HRESULT *phr);
	~FrameBuffer();

	Queue *FrameQueue() {return m_pQueue;}

	// IMediaFilter
	STDMETHODIMP Run(REFERENCE_TIME tStart);
	STDMETHODIMP Pause(void);
	STDMETHODIMP Stop(void);

private:
	Queue *m_pQueue;
	FrameBufferStream *m_pStream;
};

#endif // FRAMEBUFFER_H
