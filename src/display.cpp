#include <QtCore>
#if defined(Q_OS_LINUX)
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
#include "fg4.h"
#endif
#include "display.h"

#if defined(Q_OS_LINUX)

void Display::_prerenderCb(void *data, uint8_t **buffer, size_t size)
{
	Display *display = (Display *)data;
	struct v4l2_buffer buf;

	memset(&buf, 0, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	buf.memory = V4L2_MEMORY_MMAP;

	if (!ioctl(display->_fd, VIDIOC_DQBUF, &buf)) {
		const Buffer &b = display->_buffers.at(buf.index);
		Q_ASSERT(b.length >= size);
		display->_bufferIndex = buf.index;
		*buffer = (uint8_t*)b.start;
	}
}

void Display::_postrenderCb(void *data, uint8_t *buffer,
  int width, int height, int pixel_pitch, size_t size, int64_t pts)
{
	Q_UNUSED(buffer);
	Q_UNUSED(width);
	Q_UNUSED(height);
	Q_UNUSED(pixel_pitch);
	Display *display = (Display *)data;
	struct v4l2_buffer buf;
	struct timespec tp;

	memset(&buf, 0, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = display->_bufferIndex;
	buf.timestamp.tv_sec = pts / 1000000;
	buf.timestamp.tv_usec = pts % 1000000;
	buf.bytesused = size;
	buf.flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	buf.field = V4L2_FIELD_NONE;

	tp.tv_sec = pts / 1000000;
	tp.tv_nsec = (pts % 1000000) * 1000;
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &tp, NULL);

	ioctl(display->_fd, VIDIOC_QBUF, &buf);
}

bool Display::mapBuffers()
{
	struct v4l2_requestbuffers req;

	memset(&req, 0, sizeof(req));
	req.count = 2;
	req.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	req.memory = V4L2_MEMORY_MMAP;

	if (ioctl(_fd, VIDIOC_REQBUFS, &req) < 0) {
		_errorString = "VIDIOC_REQBUFS: " + QString(strerror(errno));
		return false;
	}
	if (req.count < 2) {
		_errorString = "VIDIOC_REQBUFS: invalid buffer count";
		return false;
	}

	for (int i = 0; i < (int)req.count; i++) {
		struct v4l2_buffer buf;

		memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if (ioctl(_fd, VIDIOC_QUERYBUF, &buf) < 0) {
			_errorString = "VIDIOC_QUERYBUF: " + QString(strerror(errno));
			return false;
		}

		void *mem = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED,
		  _fd, buf.m.offset);
		if (mem == MAP_FAILED) {
			_errorString = "mmap() error";
			return false;
		}

		_buffers.append(Buffer(mem, buf.length));
	}

	return true;
}

void Display::unmapBuffers()
{
	for (int i = 0; i < _buffers.size(); i++)
		munmap(_buffers.at(i).start, _buffers.at(i).length);

	_buffers.clear();
}

bool Display::start()
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

	for (int i = 0; i < _buffers.size(); i++) {
		struct v4l2_buffer buf;

		memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		if (ioctl(_fd, VIDIOC_QBUF, &buf) < 0) {
			_errorString = "VIDIOC_QBUF: " + QString(strerror(errno));
			return false;
		}
	}

	if (ioctl(_fd, VIDIOC_STREAMON, &type) < 0) {
		_errorString = "VIDIOC_STREAMON: " + QString(strerror(errno));
		return false;
	}

	return true;
}

void Display::stop()
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

	ioctl(_fd, VIDIOC_STREAMOFF, &type);
}

QSize Display::size()
{
	struct v4l2_format fmt;
	fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

	if (ioctl(_fd, VIDIOC_G_FMT, &fmt) < 0) {
		_errorString = "VIDIOC_G_FMT: " + QString(strerror(errno));
		return QSize();
	}

	return QSize(fmt.fmt.pix.width, fmt.fmt.pix.height);
}

bool Display::open()
{
	Q_ASSERT(_fd < 0);

	_fd = ::open(_dev.name().toLocal8Bit().constData(), O_RDWR, 0);
	if (_fd < 0) {
		_errorString = "open(): " + QString(strerror(errno));
		return false;
	}

	if (!mapBuffers()) {
		::close(_fd);
		_fd = -1;
		unmapBuffers();
		return false;
	}

	return true;
}

void Display::close()
{
	if (_fd >= 0) {
		::close(_fd);
		_fd = -1;
	}
	unmapBuffers();
}

Display::Display() : _fd(-1), _bufferIndex(-1)
{

}

Display::Display(const Device &dev) : _dev(dev), _fd(-1), _bufferIndex(-1)
{

}

Display::~Display()
{
	close();
}

#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)

void Display::_prerenderCb(void *data, uint8_t **buffer, size_t size)
{
	Q_UNUSED(data);
	Q_UNUSED(buffer);
	Q_UNUSED(size);
}

void Display::_postrenderCb(void *data, uint8_t *buffer,
  int width, int height, int pixel_pitch, size_t size, int64_t pts)
{
	Q_UNUSED(data);
	Q_UNUSED(buffer);
	Q_UNUSED(width);
	Q_UNUSED(height);
	Q_UNUSED(pixel_pitch);
	Q_UNUSED(size);
	Q_UNUSED(pts);
}

Display::Display()
{

}

Display::Display(const Device &output) : _dev(output)
{

}

Display::~Display()
{

}

bool Display::open()
{
	_errorString = "Video output not supported on Windows";
	return false;
}

void Display::close()
{

}

QSize Display::size()
{
	retirn QSize();
}

bool Display::start()
{
	return false;
}

void Display::stop()
{

}
#endif
