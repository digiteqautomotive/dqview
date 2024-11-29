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

QString Camera::url()
{
	QString url;
	QString chroma;

#if defined(Q_OS_LINUX)
	if (device().format() == RGB)
		chroma = " :v4l2-chroma=RV32";
	else if (device().format() == YUV)
		chroma = " :v4l2-chroma=YUYV";

	url = QString("v4l2://%1%2")
	  .arg(_cameraInfo.device().name(), chroma);
#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	QSize r(resolution());

	if (device()->format() == RGB)
		chroma = " :dshow-chroma=RV32";
	else if (device()->format() == YUV)
		chroma = " :dshow-chroma=YUYV";

	if (r.width() > 0 && r.height() > 0)
		url = QString("dshow:// :dshow-vdev=%1 :dshow-adev=none :dshow-aspect-ratio=%2%3")
		  .arg(_cameraInfo->device()->name(), QString::number(r.width()) + ":"
		  + QString::number(r.height()), chroma);
	else
		url = QString("dshow:// :dshow-vdev=%1 :dshow-adev=none%2")
		  .arg(_cameraInfo->device()->name(), chroma);
#else
#error "unsupported platform"
#endif

	return url;
}

Camera::Camera(DeviceInfo *cameraInfo, QObject *parent)
  : Video(parent), _cameraInfo(cameraInfo)
{
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	_config = (IFG4InputConfig*)device()->config();
#endif
}

Camera::~Camera()
{
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	if (_config)
		_config->Release();
#endif
}
