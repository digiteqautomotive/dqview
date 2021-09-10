#ifndef CAMERA_H
#define CAMERA_H

#include <QUrl>
#include "camerainfo.h"
#include "video.h"

class Camera : public Video
{
	Q_OBJECT

public:
	Camera(const CameraInfo &cameraInfo, QObject *parent = 0)
	  : Video(parent), _cameraInfo(cameraInfo) {}

	QString url() const;
	QString name() const {return _cameraInfo.name();}
	Device device() const {return _cameraInfo.device();}

private:
	CameraInfo _cameraInfo;
};

#endif // CAMERA_H
