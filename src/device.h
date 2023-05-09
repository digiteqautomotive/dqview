#ifndef DEVICE_H
#define DEVICE_H

#include <QString>

class Device {
public:
	enum Type {Unknown, Input, Output};

	Device() : _type(Unknown), _id(-1) {}
	Device(Type type, int id, const QString &name)
	  : _type(type), _id(id), _name(name) {}

	Type type() const {return _type;}
	int id() const {return _id;}
	const QString &name() const {return _name;}

	bool isValid() const {return _type > Unknown && _id >= 0;}

	bool operator==(const Device &other) const
	  {return (_type == other._type && _name == other._name);}

#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	void *config() const;
#endif

private:
	Type _type;
	int _id;
	QString _name;
};

#endif // DEVICE_H
