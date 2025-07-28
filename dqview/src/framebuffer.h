#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <cstdint>
#include <mutex>
#include <queue>
#include "streams.h"
#include "pixelformat.h"

DEFINE_GUID(CLSID_FrameBuffer, 0xfd501041, 0x8ebe, 0x11ce, 0x81, 0x83, 0x00,
  0xaa, 0x00, 0x57, 0x7d, 0xa1);

class FrameBufferStream;

class FrameBuffer: public CSource
{
public:
	DECLARE_IUNKNOWN;

	struct Frame
	{
	public:
		Frame(size_t size) : m_size(size), m_ts(0)
		{
			m_pBuffer = new char[size];
		}
		~Frame() {delete[] m_pBuffer;}

		size_t size() const {return m_size;}
		int64_t TimeStamp() const {return m_ts;}
		void SetTimeStamp(int64_t ts) {m_ts = ts;}
		char *Buffer() {return m_pBuffer;}

	private:
		size_t m_size;
		int64_t m_ts;
		char *m_pBuffer;
	};

	class Queue {
	public:
		inline Queue(size_t capacity) : _capacity(capacity) {}

		bool push(Frame *elem)
		{
			bool ret = false;

			_mutex.lock();
			if (_queue.size() < _capacity) {
				_queue.push(elem);
				ret = true;
			}
			_mutex.unlock();

			return ret;
		}

		bool pop()
		{
			bool ret = false;

			_mutex.lock();
			if (_queue.size()) {
				_queue.pop();
				ret = true;
			}
			_mutex.unlock();

			return ret;
		}

		Frame *top()
		{
			Frame *ret;

			_mutex.lock();
			ret = (_queue.size()) ? _queue.front() : 0;
			_mutex.unlock();

			return ret;
		}

	private:
		size_t _capacity;
		std::queue<Frame*> _queue;
		std::mutex _mutex;
	};

	FrameBuffer(PixelFormat Format, int iWidth, int iHeight, int iCapacity,
	  HRESULT *phr);
	~FrameBuffer();

	int Width() const {return m_iWidth;}
	int Height() const {return m_iHeight;}
	PixelFormat Format() const {return m_Format;}
	Queue &FrameQueue() {return m_pQueue;}

private:
	int m_iWidth, m_iHeight;
	PixelFormat m_Format;
	Queue m_pQueue;
	FrameBufferStream *m_pStream;
};

#endif // FRAMEBUFFER_H
