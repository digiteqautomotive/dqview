#include <QtGlobal>
#include "camera.h"
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
#include "fg4.h"
#endif

#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
QPoint Camera::resolution() const
{
	long resolution;

	IFG4InputConfig *config = (IFG4InputConfig*)device().config();
	if (!config || FAILED(config->GetDetectedResolution(&resolution)))
		return QPoint();

	return QPoint(resolution >> 16, resolution & 0xFFFF);
}
#endif

QString Camera::url() const
{
	QString url;
#if defined(Q_OS_LINUX)
	url = QString("v4l2://%1").arg(_cameraInfo.device().name());
#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	QPoint r(resolution());
	if (r.isNull())
		url = QString("dshow:// :dshow-vdev=%1 :dshow-adev=none")
		  .arg(_cameraInfo.device().name());
	else
		url = QString("dshow:// :dshow-vdev=%1 :dshow-adev=none :dshow-aspect-ratio=%2")
		  .arg(_cameraInfo.device().name(), QString::number(r.x()) + ":"
		  + QString::number(r.y()));
#else
#error "unsupported platform"
#endif
	return url;
}
