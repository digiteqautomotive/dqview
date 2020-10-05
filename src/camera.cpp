#include <QtGlobal>
#include "camera.h"

QString Camera::url() const
{
	QString url;
#if defined(Q_OS_LINUX)
	url = QString("v4l2://%1").arg(_cameraInfo.deviceName());
#elif defined(Q_OS_WIN)
	url = QString("dshow://%1").arg(_cameraInfo.deviceName());
#else
#error "unsupported platform"
#endif

	return url;
}

QString Camera::name() const
{
	return _cameraInfo.description();
}
