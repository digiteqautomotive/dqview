#include <QtGlobal>
#include "camera.h"

#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
QSize Camera::resolution() const
{
	long resolution;

	if (!_config)
		return QSize();
	return (FAILED(_config->GetDetectedResolution(&resolution)))
	  ? QSize() : QSize(resolution >> 16, resolution & 0xFFFF);
}
#endif

QString Camera::url() const
{
	QString url;

#if defined(Q_OS_LINUX)
	url = QString("v4l2://%1").arg(_cameraInfo.device().name());
#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	QSize r(resolution());

	if (r.width() > 0 && r.height() > 0)
		url = QString("dshow:// :dshow-vdev=%1 :dshow-adev=none :dshow-aspect-ratio=%2")
		  .arg(_cameraInfo.device().name(), QString::number(r.width()) + ":"
		  + QString::number(r.height()));
	else
		url = QString("dshow:// :dshow-vdev=%1 :dshow-adev=none")
		  .arg(_cameraInfo.device().name());
#else
#error "unsupported platform"
#endif

	return url;
}

Camera::Camera(const DeviceInfo &cameraInfo, QObject *parent)
  : Video(parent), _cameraInfo(cameraInfo)
{
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	_config = (IFG4InputConfig*)device().config();
#endif
}

Camera::~Camera()
{
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	if (_config)
		_config->Release();
#endif
}
