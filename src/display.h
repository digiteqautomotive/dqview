#ifndef DISPLAY_H
#define DISPLAY_H

#include <QVector>
#include <QSize>
#include <vlc/vlc.h>
#include "device.h"

class Display
{
public:
	typedef void (*PrerenderCallback) (void *data, uint8_t **buffer, size_t size);
	typedef void (*PostrenderCallback) (void *data, uint8_t *buffer, int width,
	  int height, int pixel_pitch, size_t size, int64_t pts);

	Display();
	Display(const Device &dev);
	~Display();

	const Device &device() const {return _dev;}

	bool open();
	void close();
	QSize size();
	bool start();
	void stop();

	static PrerenderCallback prerender() {return _prerenderCb;}
	static PostrenderCallback postrender() {return _postrenderCb;}

	const QString &errorString() {return _errorString;}

private:
	static void _prerenderCb(void *data, uint8_t **buffer, size_t size);
	static void _postrenderCb(void *data, uint8_t *buffer, int width,
	  int height, int pixel_pitch, size_t size, int64_t pts);

	Device _dev;
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

	int _fd, _bufferIndex;
	QVector<Buffer> _buffers;
#endif
};

#endif // DISPLAY_H
