#ifndef CAMERA_H
#define CAMERA_H

#include <QUrl>
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
	DeviceInfo _cameraInfo;
};

#endif // CAMERA_H
