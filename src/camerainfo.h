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
	int id() const {return _id;}

	bool isNull() {return _device.isNull();}

	bool operator==(const CameraInfo &other) const
	  {return (_device == other._device);}
	bool operator<(const CameraInfo &other) const
	  {return (_device < other._device);}

	static QList<CameraInfo> availableCameras();

private:
	CameraInfo() : _id(-1) {}
	CameraInfo(const QString &device, int id, const QString &description = QString())
	  : _device(device), _description(description), _id(id) {}

#if defined(Q_OS_LINUX)
	static CameraInfo cameraInfo(const QString &device, int *id);
#endif // Q_OS_LINUX

	QString _device;
	QString _description;
	int _id;
};

#endif // CAMERAINFO_H
