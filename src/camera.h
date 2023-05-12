#ifndef CAMERA_H
#define CAMERA_H

#include <QUrl>
#include <QSize>
#include "deviceinfo.h"
#include "video.h"
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
#include "fg4.h"
#endif

class Camera : public Video
{
	Q_OBJECT

public:
	Camera(const DeviceInfo &cameraInfo, QObject *parent = 0);
	~Camera();

	QString url() const;
	QString name() const {return _cameraInfo.name();}
	Device device() const {return _cameraInfo.device();}

private:
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	QSize resolution() const;

	IFG4InputConfig *_config;
#endif
	DeviceInfo _cameraInfo;
};

#endif // CAMERA_H
