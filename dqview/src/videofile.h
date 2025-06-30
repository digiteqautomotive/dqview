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
	bool isValid() const {return !_path.isEmpty();}

#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	QString url() {return "file:///" + _path;}
#else
	QString url() {return "file://" + _path;}
#endif
	QString name() const {return QFileInfo(_path).fileName();}

private:
	QString _path;
};

#endif // VIDEOFILE_H
