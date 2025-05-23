#ifndef SCREENCAPTURE_H
#define SCREENCAPTURE_H

#include <QFileInfo>
#include <QSize>
#include <QPoint>
#include "deviceinfo.h"
#include "video.h"

class ScreenCapture : public Video
{
	Q_OBJECT

public:
	ScreenCapture(QObject *parent = 0) : Video(parent), _fullScreen(true) {}

	void captureFullscreen(unsigned fps) {_fps = fps, _fullScreen = true;}
	void captureRegion(unsigned fps, const QSize &size, const QPoint &pos)
	  {_fps = fps, _size = size, _pos = pos; _fullScreen = false;}

	bool show() const {return false;}
	QString url()
	{
		return (_fullScreen)
		  ? QString("screen:// :screen-fps=%1").arg(QString::number(_fps))
		  : QString("screen:// :screen-fps=%1 :screen-width=%2 :screen-height=%3"
			" :screen-top=%4 :screen-left=%5").arg(QString::number(_fps),
			QString::number(_size.width()), QString::number(_size.height()),
			QString::number(_pos.y()), QString::number(_pos.x()));
	}
	QString name() const {return "Screen Capture";}

private:
	QSize _size;
	QPoint _pos;
	unsigned _fps;
	bool _fullScreen;
};

#endif // SCREENCAPTURE_H
