#include <QtGlobal>
#if defined(Q_OS_LINUX)
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
#include <control.h>
#include "fg4.h"
#endif
#include "videooutput.h"

/* The amount of frames (VLC callbacks) we must accept without blocking to not
   jerk the playback (VLC decodes the frames in bursts and the sound gets
   distorted if not enaugh callbacks are processed). */
#define FRAME_BUFFERS 25

#if defined(Q_OS_LINUX)

void VideoOutput::_prerenderCb(void *data, uint8_t **buffer, size_t size)
{
	VideoOutput *display = (VideoOutput *)data;
	struct v4l2_buffer buf;

	memset(&buf, 0, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	buf.memory = V4L2_MEMORY_MMAP;

	if (display->_usedBuffers < display->_buffers.size()) {
		const Buffer &b = display->_buffers.at(display->_usedBuffers);
		Q_ASSERT(b.length >= size);
		display->_bufferIndex = display->_usedBuffers;
		*buffer = (uint8_t*)b.start;
		display->_usedBuffers++;
	} else {
		ioctl(display->_fd, VIDIOC_DQBUF, &buf);
		const Buffer &b = display->_buffers.at(buf.index);
		Q_ASSERT(b.length >= size);
		display->_bufferIndex = buf.index;
		*buffer = (uint8_t*)b.start;
	}
}

void VideoOutput::_postrenderCb(void *data, uint8_t *buffer,
  int width, int height, int pixel_pitch, size_t size, int64_t pts)
{
	Q_UNUSED(buffer);
	Q_UNUSED(width);
	Q_UNUSED(height);
	Q_UNUSED(pixel_pitch);
	VideoOutput *display = (VideoOutput *)data;
	struct v4l2_buffer buf;

	memset(&buf, 0, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = display->_bufferIndex;
	buf.timestamp.tv_sec = pts / 1000000;
	buf.timestamp.tv_usec = pts % 1000000;
	buf.bytesused = size;
	buf.flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	buf.field = V4L2_FIELD_NONE;

	ioctl(display->_fd, VIDIOC_QBUF, &buf);
}

bool VideoOutput::mapBuffers()
{
	struct v4l2_requestbuffers req;

	memset(&req, 0, sizeof(req));
	req.count = FRAME_BUFFERS;
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

void VideoOutput::unmapBuffers()
{
	for (int i = 0; i < _buffers.size(); i++)
		munmap(_buffers.at(i).start, _buffers.at(i).length);

	_buffers.clear();
}

bool VideoOutput::start(unsigned num, unsigned den)
{
	struct v4l2_streamparm parm;

	memset(&parm, 0, sizeof(parm));
	parm.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	parm.parm.output.capability = V4L2_CAP_TIMEPERFRAME;
	parm.parm.output.timeperframe.numerator = num;
	parm.parm.output.timeperframe.denominator = den;

	if (ioctl(_fd, VIDIOC_S_PARM, &parm) < 0) {
		_errorString = "VIDIOC_S_PARM: " + QString(strerror(errno));
		return false;
	}

	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

	if (ioctl(_fd, VIDIOC_STREAMON, &type) < 0) {
		_errorString = "VIDIOC_STREAMON: " + QString(strerror(errno));
		return false;
	}

	_usedBuffers = 0;

	return true;
}

void VideoOutput::stop()
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

	ioctl(_fd, VIDIOC_STREAMOFF, &type);
}

QSize VideoOutput::size()
{
	struct v4l2_format fmt;
	fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

	if (ioctl(_fd, VIDIOC_G_FMT, &fmt) < 0) {
		_errorString = "VIDIOC_G_FMT: " + QString(strerror(errno));
		return QSize();
	}

	return QSize(fmt.fmt.pix.width, fmt.fmt.pix.height);
}

PixelFormat VideoOutput::format()
{
	struct v4l2_format fmt;
	fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

	if (ioctl(_fd, VIDIOC_G_FMT, &fmt) < 0) {
		_errorString = "VIDIOC_G_FMT: " + QString(strerror(errno));
		return UnknownFormat;
	}

	switch (fmt.fmt.pix.pixelformat) {
		case V4L2_PIX_FMT_ABGR32:
			return RGB;
		case V4L2_PIX_FMT_YUYV:
			return YUV;
		default:
			_errorString = QString("%1: Unknown fourcc")
			  .arg(fmt.fmt.pix.pixelformat);
			return UnknownFormat;
	}
}

bool VideoOutput::open()
{
	Q_ASSERT(_fd < 0);

	_fd = ::open(_dev->name().toLocal8Bit().constData(), O_RDWR, 0);
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

void VideoOutput::close()
{
	if (_fd >= 0) {
		::close(_fd);
		_fd = -1;
	}
	unmapBuffers();
}

VideoOutput::VideoOutput()
  : _dev(0), _fd(-1), _bufferIndex(-1), _usedBuffers(-1)
{

}

VideoOutput::VideoOutput(Device *dev)
  : _dev(dev), _fd(-1), _bufferIndex(-1), _usedBuffers(-1)
{

}

VideoOutput::~VideoOutput()
{
	close();
}

#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)

void VideoOutput::_prerenderCb(void *data, uint8_t **buffer, size_t size)
{
	VideoOutput *display = (VideoOutput *)data;
	FrameBuffer::Queue *queue = display->_frameBuffer->FrameQueue();

	Q_ASSERT((size_t)(queue->Width() *  queue->Height() * 4) >= size);

	while (true) {
		queue->Lock();
		if (queue->IsFull()) {
			queue->Unlock();
			Sleep(10);
		} else
			break;
	};

	*buffer = (uint8_t*)queue->Write()->Buffer();
}

void VideoOutput::_postrenderCb(void *data, uint8_t *buffer,
  int width, int height, int pixel_pitch, size_t size, int64_t pts)
{
	Q_UNUSED(buffer);
	Q_UNUSED(size);
	Q_UNUSED(width);
	Q_UNUSED(height);
	Q_UNUSED(pixel_pitch);
	VideoOutput *display = (VideoOutput *)data;
	FrameBuffer::Queue *queue = display->_frameBuffer->FrameQueue();

	queue->Write()->SetTimeStamp(pts);
	queue->Push();
	queue->Unlock();
}

VideoOutput::VideoOutput()
  : _dev(0), _frameBuffer(0), _graph(0), _graphbuilder(0), _capbuilder(0)
{

}

VideoOutput::VideoOutput(Device *output)
  : _dev(output), _frameBuffer(0), _graph(0), _graphbuilder(0), _capbuilder(0)
{
}

VideoOutput::~VideoOutput()
{

}

bool VideoOutput::open()
{
	HRESULT hr;
	IPin *CamPIN;
	IPin *RenderPIN;
	IEnumPins *pEnum;

	QSize s(size());

	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
	  IID_IMediaControl, reinterpret_cast<void**>(&_graph));
	if (FAILED(hr) || !_graph) {
		_errorString = "Failed to create filter graph";
		return false;
	}

	hr = _graph->QueryInterface(IID_IGraphBuilder,
	  reinterpret_cast<void**>(&_graphbuilder));
	if (FAILED(hr) || !_graphbuilder) {
		_errorString = "Failed to get graph builder interface";
		_graph->Release();
		return false;
	}

	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC,
	  IID_ICaptureGraphBuilder2, reinterpret_cast<void**>(&_capbuilder));
	if (FAILED(hr) || !_capbuilder) {
		_errorString = "Failed to create capture graph builder";
		_graphbuilder->Release();
		_graph->Release();
		return false;
	}

	hr = _capbuilder->SetFiltergraph(_graphbuilder);
	if (FAILED(hr)) {
		_errorString = "Failed to attach filter graph to the capture graph builder";
		_capbuilder->Release();
		_graphbuilder->Release();
		_graph->Release();
		return false;
	}

	_frameBuffer = new FrameBuffer(s.width(), s.height(), FRAME_BUFFERS, &hr);
	_frameBuffer->AddRef();
	if (FAILED(hr)) {
		_errorString = "Error creating renderer filter";
		_frameBuffer->Release();
		_capbuilder->Release();
		_graphbuilder->Release();
		_graph->Release();
		return false;
	}

	hr = _graphbuilder->AddFilter(_frameBuffer, L"Camera");
	if (FAILED(hr)) {
		_errorString = "Failed to add camera filter to filter graph";
		_frameBuffer->Release();
		_capbuilder->Release();
		_graphbuilder->Release();
		_graph->Release();
		return false;
	}

	hr = _graphbuilder->AddFilter(_dev->filter(), L"Renderer");
	if (FAILED(hr)) {
		_errorString = "Failed to add renderer filter to filter graph";
		_frameBuffer->Release();
		_capbuilder->Release();
		_graphbuilder->Release();
		_graph->Release();
		return false;
	}

	_frameBuffer->EnumPins(&pEnum);
	hr = pEnum->Next(1, &CamPIN, NULL);
	pEnum->Release();
	if (FAILED(hr)) {
		_errorString = "Cannot obtain output pin from camera";
		_frameBuffer->Release();
		_capbuilder->Release();
		_graphbuilder->Release();
		_graph->Release();
		return false;
	}

	_dev->filter()->EnumPins(&pEnum);
	hr = pEnum->Next(1, &RenderPIN, NULL);
	pEnum->Release();
	if (FAILED(hr)) {
		_errorString = "Cannot obtain output pin from renderer";
		_frameBuffer->Release();
		_capbuilder->Release();
		_graphbuilder->Release();
		_graph->Release();
		return false;
	}

	hr = _graphbuilder->Connect(CamPIN, RenderPIN);
	if (FAILED(hr)) {
		_errorString = "Cannot connect renderer with grabber";
		_frameBuffer->Release();
		_capbuilder->Release();
		_graphbuilder->Release();
		_graph->Release();
		return false;
	}

	return true;
}

void VideoOutput::close()
{
	_frameBuffer->Release();
	_capbuilder->Release();
	_graphbuilder->Release();
	_graph->Release();
}

QSize VideoOutput::size()
{
	long resolution;
	IFG4OutputConfig *config = (IFG4OutputConfig*)_dev->config();
	if (!config)
		return QSize();

	config->GetResolution(&resolution);
	return QSize(resolution >> 16, resolution & 0xFFFF);
}

PixelFormat VideoOutput::format()
{
	return RGB;
}

bool VideoOutput::start(unsigned num, unsigned den)
{
	Q_UNUSED(num);
	Q_UNUSED(den);

	_frameBuffer->Run(0);
	_dev->filter()->Run(0);

	return true;
}

void VideoOutput::stop()
{
	_dev->filter()->Stop();
	_frameBuffer->Stop();
}
#endif
