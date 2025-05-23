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
	QString transform;
	QString aspectRatio;

	bool resize;
	bool fullScreen;
	QByteArray windowGeometry;
	QByteArray windowState;

	int screenWidth;
	int screenHeight;
	int screenTop;
	int screenLeft;
	int screenFPS;
	bool screenFull;
};

#endif // OPTIONS_H
