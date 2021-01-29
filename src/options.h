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
	bool flip;

	bool resize;
	bool fullScreen;
	QByteArray windowGeometry;
	QByteArray windowState;
};

#endif // OPTIONS_H
