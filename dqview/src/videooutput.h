#ifndef VIDEOOUTPUT_H
#define VIDEOOUTPUT_H

#include <QVector>
#include <QSize>
#include <vlc/vlc.h>
#include "device.h"
#include "pixelformat.h"
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
#include "framebuffer.h"
#endif

class VideoOutput
{
public:
	typedef void (*PrerenderCallback) (void *data, uint8_t **buffer, size_t size);
	typedef void (*PostrenderCallback) (void *data, uint8_t *buffer, int width,
	  int height, int pixel_pitch, size_t size, int64_t pts);

	VideoOutput();
	VideoOutput(Device *dev);
	~VideoOutput();

	Device *device() {return _dev;}

	bool open(unsigned num = 0, unsigned den = 0);
	void close();
	QSize size();
	PixelFormat format();
	bool start();
	void stop();

	static PrerenderCallback prerender() {return _prerenderCb;}
	static PostrenderCallback postrender() {return _postrenderCb;}

	const QString &errorString() {return _errorString;}

private:
	static void _prerenderCb(void *data, uint8_t **buffer, size_t size);
	static void _postrenderCb(void *data, uint8_t *buffer, int width,
	  int height, int pixel_pitch, size_t size, int64_t pts);

	Device *_dev;
	QString _errorString;

#if defined(Q_OS_LINUX)

	struct Buffer {
		Buffer() : start(NULL), length(0) {}
		Buffer(void *start, size_t length) : start(start), length(length) {}

		void *start;
		size_t length;
	};

	bool mapBuffers();
	void unmapBuffers();

	int _fd, _bufferIndex, _usedBuffers;
	QVector<Buffer> _buffers;

#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)

	HRESULT FG4MediaType(REFGUID subType, AM_MEDIA_TYPE **ppmt);

	FrameBuffer *_frameBuffer;
	IMediaControl *_graph;
	int _bufferIndex;
	QVector<FrameBuffer::Frame*> _buffers;

#endif
};

#endif // VIDEOOUTPUT_H
