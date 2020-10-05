#include <QTemporaryFile>
#include <QDir>
#include "stream.h"

#define ARRAY_SIZE(a) ((int)(sizeof(a) / sizeof(*(a))))

static struct {
	const char *name;
	const char *url;
	const char *sdp;
} formats[] = {
	{"H264-MPEGTS-TCP", "tcp://%1:%2", 0},
	{"H264-RTP-UDP", 0, "a=type:broadcast\nm=video %2 RTP/AVP 96\nc=IN IP4 %1\na=rtpmap:96 H264/90000"},
	{"JPEG-MUX-TCP", "tcp://%1:%2", 0},
	{"JPEG-RTP-UDP", 0, "a=type:broadcast\nm=video %2 RTP/AVP 96\nc=IN IP4 %1\na=rtpmap:96 JPEG/90000"}
};

Stream::Stream(const StreamInfo &streamInfo, QObject *parent)
  : Video(parent), _streamInfo(streamInfo)
{
	for (int i = 0; i < ARRAY_SIZE(formats); i++) {
		if (_streamInfo.type() == formats[i].name && formats[i].sdp) {
			QString tpl(QDir::tempPath() + "/" + QString(formats[i].name)
			  + QString("-XXXXXX.sdp"));
			QTemporaryFile *tmp = new QTemporaryFile(tpl, this);
			QString sdp(QString(formats[i].sdp).arg(_streamInfo.address(),
			  QString::number(_streamInfo.port())));
			QByteArray ba(sdp.toLatin1());

			tmp->open();
			tmp->write(ba);
			tmp->close();

			_sdp = tmp->fileName();

			break;
		}
	}
}

QString Stream::url() const
{
	for (int i = 0; i < ARRAY_SIZE(formats); i++) {
		if (_streamInfo.type() == formats[i].name) {
			if (formats[i].url)
				return QString(formats[i].url).arg(_streamInfo.address(),
				  QString::number(_streamInfo.port()));
			else
				return QString("file://%1").arg(_sdp);
		}
	}

	return QString();
}

QString Stream::name() const
{
	return _streamInfo.address() + ":" + QString::number(_streamInfo.port())
	  + " " + _streamInfo.type();
}

QStringList Stream::types()
{
	QStringList list;

	for (int i = 0; i < ARRAY_SIZE(formats); i++)
		list.append(formats[i].name);

	return list;
}
