#ifndef CAMERA_H
#define CAMERA_H

#include <QUrl>
#include <QPoint>
#include "deviceinfo.h"
#include "video.h"

class Camera : public Video
{
	Q_OBJECT

public:
	Camera(const DeviceInfo &cameraInfo, QObject *parent = 0)
	  : Video(parent), _cameraInfo(cameraInfo) {}

	QString url() const;
	QString name() const {return _cameraInfo.name();}
	Device device() const {return _cameraInfo.device();}

private:
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	QPoint resolution() const;
#endif
	DeviceInfo _cameraInfo;
};

#endif // CAMERA_H
