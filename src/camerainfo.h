#ifndef CAMERAINFO_H
#define CAMERAINFO_H

#include <QList>
#include "device.h"

class CameraInfo
{
public:
	const Device &device() const {return _device;}
	QString name() const
	{
		return _description.isEmpty()
		  ? _device.name() : _device.name() + " - " + _description;
	}

	bool isNull() {return _device.name().isNull();}

	bool operator==(const CameraInfo &other) const
	  {return (_device == other._device);}
	bool operator<(const CameraInfo &other) const
	  {return (_device.name() < other._device.name());}

	static QList<CameraInfo> availableCameras();

private:
	CameraInfo() {}
	CameraInfo(const Device &device, const QString &description = QString())
	  : _device(device), _description(description) {}

#if defined(Q_OS_LINUX)
	static CameraInfo cameraInfo(const QString &device, int *id);
#endif // Q_OS_LINUX

	Device _device;
	QString _description;
};

#endif // CAMERAINFO_H
