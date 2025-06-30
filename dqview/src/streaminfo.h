#ifndef STREAMINFO_H
#define STREAMINFO_H

#include <QString>
#include <QDataStream>

class StreamInfo
{
public:
	StreamInfo() : _port(0) {}
	StreamInfo(const QString &address, quint16 port, const QString type)
	  : _address(address), _port(port), _type(type) {}

	const QString &address() const {return _address;}
	int port() const {return _port;}
	const QString &type() const {return _type;}

	bool isNull() {return (_address.isNull() && _port == 0);}

	bool operator==(const StreamInfo &other) const
	{
		return (_address == other._address && _port == other._port
		  && _type == other.type());
	}


private:
	friend QDataStream& operator>>(QDataStream &in, StreamInfo &info);

	QString _address;
	int _port;
	QString _type;
};


Q_DECLARE_METATYPE(StreamInfo)

inline QDataStream &operator<<(QDataStream &out, const StreamInfo &info)
{
	out << info.address() << info.port() << info.type();
	return out;
}

inline QDataStream& operator>>(QDataStream &in, StreamInfo &info)
{
	in >> info._address;
	in >> info._port;
	in >> info._type;
	return in;
}

#endif // STREAMINFO_H
