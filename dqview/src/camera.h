#ifndef CAMERA_H
#define CAMERA_H

#include <QSize>
#include "deviceinfo.h"
#include "video.h"
#if defined(Q_OS_WIN32)
#include "fg4.h"
#endif

class Camera : public Video
{
	Q_OBJECT

public:
	Camera(DeviceInfo *cameraInfo, QObject *parent = 0);
	~Camera();

	QString url();
	QString name() const {return _cameraInfo->name();}
	Device *device() {return _cameraInfo->device();}

private:
#if defined(Q_OS_WIN32)
	QSize resolution() const;

	IFG4InputConfig *_config;
#endif
	DeviceInfo *_cameraInfo;
};

#endif // CAMERA_H
