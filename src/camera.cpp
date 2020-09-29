#include "camera.h"

QString Camera::url() const
{
	return QString("avdevice:video4linux2:%1").arg(_cameraInfo.deviceName());
}

QString Camera::name() const
{
	return _cameraInfo.description();
}
