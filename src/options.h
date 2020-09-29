#ifndef OPTIONS_H
#define OPTIONS_H

#include <QString>
#include <QList>
#include "streaminfo.h"

struct Options
{
	QString videoDir;
	QString imagesDir;
	QList<StreamInfo> streams;

	bool resize;
};

#endif // OPTIONS_H
