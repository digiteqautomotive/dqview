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

	void captureFullscreen() {_fullScreen = true;}
	void captureRegion(const QSize &size, const QPoint &pos)
	  {_size = size, _pos = pos; _fullScreen = false;}

	bool show() const {return false;}
	QString url()
	{
		return (_fullScreen)
		  ? "screen:// :screen-fps=30"
		  : QString("screen:// :screen-fps=30 :screen-width=%1 :screen-height=%2"
			" :screen-top=%3 :screen-left=%4").arg(QString::number(_size.width()),
			QString::number(_size.height()), QString::number(_pos.y()),
			QString::number(_pos.x()));
	}
	QString name() const {return "Screen Capture";}

private:
	QSize _size;
	QPoint _pos;
	bool _fullScreen;
};

#endif // SCREENCAPTURE_H
