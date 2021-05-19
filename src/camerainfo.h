#ifndef CAMERAINFO_H
#define CAMERAINFO_H

#include <QList>

class CameraInfo
{
public:
	const QString &device() const {return _device;}
	QString name() const
	{
		return _description.isEmpty()
		  ? _device : _device + " - " + _description;
	}
	bool isMGB4() const {return _mgb4;}

	bool isNull() {return _device.isNull();}

	bool operator==(const CameraInfo &other) const
	  {return (_device == other._device);}
	bool operator<(const CameraInfo &other) const
	  {return (_device < other._device);}

	static QList<CameraInfo> availableCameras();

private:
	CameraInfo() : _mgb4(false) {}
	CameraInfo(const QString &device, const QString &description, bool mgb4)
	  : _device(device), _description(description), _mgb4(mgb4) {}

#if defined(Q_OS_LINUX)
	static CameraInfo cameraInfo(const QString &device);
#endif // Q_OS_LINUX

	QString _device;
	QString _description;
	bool _mgb4;
};

#endif // CAMERAINFO_H
