#ifndef CAMERAINFO_H
#define CAMERAINFO_H

#include <QList>

class CameraInfo
{
public:
	const QString &deviceName() const {return _deviceName;}
	const QString &description() const {return _description;}

	bool isNull() {return _deviceName.isNull();}

	static QList<CameraInfo> availableCameras();

private:
	CameraInfo() {}
	CameraInfo(const QString &device, const QString &description)
	  : _deviceName(device), _description(description) {}

#if defined(Q_OS_LINUX)
	static CameraInfo cameraInfo(const QString &device);
#endif // Q_OS_LINUX

	QString _deviceName;
	QString _description;
};

#endif // CAMERAINFO_H
