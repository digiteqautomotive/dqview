#include <QtGlobal>
#include "camera.h"

QString Camera::url() const
{
	QString url;
#if defined(Q_OS_LINUX)
	url = QString("v4l2://%1").arg(_cameraInfo.device());
#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	url = QString("dshow:// :dshow-vdev=%1 :dshow-adev=none")
	  .arg(_cameraInfo.deviceName());
#else
#error "unsupported platform"
#endif

	return url;
}

QString Camera::device() const
{
	return _cameraInfo.isMGB4() ? _cameraInfo.device() : QString();
}
