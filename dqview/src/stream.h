#ifndef STREAM_H
#define STREAM_H

#include <QString>
#include "video.h"
#include "streaminfo.h"

class Stream : public Video
{
	Q_OBJECT

public:
	Stream(const StreamInfo &streamInfo, QObject *parent = 0);

	QString url();
	QString name() const;
	Device *device() {return &_device;}

	const StreamInfo &info() const {return _streamInfo;}

	static QStringList types();

private:
	Device _device;
	StreamInfo _streamInfo;
	QString _sdp;
};

#endif // STREAM_H
