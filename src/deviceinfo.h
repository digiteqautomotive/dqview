#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QList>
#include "device.h"

class DeviceInfo
{
public:
	Device *device() {return &_device;}
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

	void setDescription(const QString &desc) {_description = desc;}

	static QList<DeviceInfo*> inputDevices();
	static QList<DeviceInfo*> outputDevices();

private:
	DeviceInfo() {}
	DeviceInfo(Device::Type type, int id, const QString &name,
	  const QString &description = QString())
	  : _device(type, id, name), _description(description) {}

#if defined(Q_OS_LINUX)
	static DeviceInfo deviceInfo(const QString &device, int *id);
	static QList<DeviceInfo> devices(Device::Type type);
#endif // Q_OS_LINUX

	Device _device;
	QString _description;
};

#endif // DEVICEINFO_H
