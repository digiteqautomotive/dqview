#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QList>
#include "device.h"

class DeviceInfo
{
public:
	const Device &device() const {return _device;}
	QString name() const
	{
		return _description.isEmpty()
		  ? _device.name() : _device.name() + " - " + _description;
	}

	bool isNull() {return _device.name().isNull();}

	bool operator==(const DeviceInfo &other) const
	  {return (_device == other._device);}
	bool operator<(const DeviceInfo &other) const
	  {return (_device.name() < other._device.name());}

	static QList<DeviceInfo> availableDevices();

private:
	DeviceInfo() {}
	DeviceInfo(const Device &device, const QString &description = QString())
	  : _device(device), _description(description) {}

#if defined(Q_OS_LINUX)
	static DeviceInfo deviceInfo(const QString &device, int *id);
#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	static QList<DeviceInfo> inputDevices();
	static QList<DeviceInfo> outputDevices();
#else
#error "unsupported platform"
#endif // Q_OS_LINUX

	Device _device;
	QString _description;
};

#endif // DEVICEINFO_H
