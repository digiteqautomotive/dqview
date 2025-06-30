#ifndef DEVICE_H
#define DEVICE_H

#include <QString>
#include <QMetaType>
#include "pixelformat.h"
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
#include <strmif.h>
#endif

class Device {
public:
	enum Type {Unknown, Input, Output};

#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	Device() :
	  _format(UnknownFormat), _type(Unknown), _id(-1), _config(0), _filter(0) {}
	~Device();
#else
	Device() : _format(UnknownFormat), _type(Unknown), _id(-1) {}
#endif
	Device(Type type, int id, const QString &name);
	Device(const Device &other) = delete;

	Type type() const {return _type;}
	int id() const {return _id;}
	const QString &name() const {return _name;}
	PixelFormat format() const {return _format;}

	bool isValid() const {return _type > Unknown && _id >= 0;}

	bool operator==(const Device &other) const
	  {return (_type == other._type && _name == other._name);}
	bool operator!=(const Device &other) const
	  {return (_type != other._type || _name != other._name);}

	void setFormat(PixelFormat format) {_format = format;}

#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	void *config();
	IBaseFilter *filter();
#endif

protected:
	PixelFormat _format;

private:
	Type _type;
	int _id;
	QString _name;
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	void *_config;
	IBaseFilter *_filter;
#endif
};

Q_DECLARE_METATYPE(Device*)

#endif // DEVICE_H
