#ifndef OPTIONS_H
#define OPTIONS_H

#include <QString>
#include <QList>
#include <QByteArray>
#include "streaminfo.h"

struct Options
{
	QString codec;
	unsigned bitrate;
	QString videoDir;
	QString imagesDir;
	QList<StreamInfo> streams;

	bool resize;
	bool fullScreen;
	bool flip;
	QByteArray windowGeometry;
	QByteArray windowState;
};

#endif // OPTIONS_H
