#ifndef VIDEOFILE_H
#define VIDEOFILE_H

#include <QFileInfo>
#include "deviceinfo.h"
#include "video.h"

class VideoFile : public Video
{
	Q_OBJECT

public:
	VideoFile(QObject *parent = 0) : Video(parent) {}

	void setFile(const QString &path) {_path = path;}
	const QString &file() const {return _path;}

	QString url() const {return "file://" + _path;}
	QString name() const {return QFileInfo(_path).fileName();}

private:
	QString _path;
};

#endif // VIDEOFILE_H
