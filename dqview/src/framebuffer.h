#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <cstdint>
#include <mutex>
#include <queue>
#include <condition_variable>
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
		inline Queue(size_t capacity, unsigned timeout)
		  : _capacity(capacity), _timeout(timeout) {}

		void push(Frame *elem)
		{
			std::unique_lock<std::mutex> lock(_mutex);

			while (_queue.size() >= _capacity)
				_write.wait(lock);

			_queue.push(elem);
			lock.unlock();

			_read.notify_one();
		}

		Frame *top()
		{
			std::cv_status status = std::cv_status::no_timeout;
			std::unique_lock<std::mutex> lock(_mutex);

			while (_queue.empty() && status != std::cv_status::timeout)
				status = _read.wait_for(lock, _timeout);

			return (status == std::cv_status::timeout) ? 0 : _queue.front();
		}

		void pop()
		{
			std::unique_lock<std::mutex> lock(_mutex);

			while (_queue.empty())
				_read.wait(lock);

			_queue.pop();
			lock.unlock();

			_write.notify_one();
		}

		bool ready()
		{
			std::cv_status status = std::cv_status::no_timeout;
			std::unique_lock<std::mutex> lock(_mutex);

			while (_queue.size() >= _capacity
			  && status != std::cv_status::timeout)
				status = _write.wait_for(lock, _timeout);

			return (status == std::cv_status::timeout) ? false : true;
		}

	private:
		size_t _capacity;
		std::chrono::milliseconds _timeout;
		std::queue<Frame*> _queue;
		std::mutex _mutex;
		std::condition_variable _read, _write;
	};

	FrameBuffer(PixelFormat Format, int iWidth, int iHeight, int iTPF,
	  int iCapacity, HRESULT *phr);
	~FrameBuffer();

	int Width() const {return m_iWidth;}
	int Height() const {return m_iHeight;}
	PixelFormat Format() const {return m_Format;}
	int TimePerFrame() const {return m_iTimePerFrame;}
	Queue &FrameQueue() {return m_pQueue;}

private:
	int m_iWidth, m_iHeight;
	int m_iTimePerFrame;
	PixelFormat m_Format;
	Queue m_pQueue;
	FrameBufferStream *m_pStream;
};

#endif // FRAMEBUFFER_H
