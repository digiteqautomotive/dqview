#ifndef VIDEO_H
#define VIDEO_H

#include <QObject>
#include <QString>
#include "device.h"

class Video : public QObject
{
	Q_OBJECT

public:
	Video(QObject *parent = 0) : QObject(parent) {}
	virtual ~Video() {}

	virtual QString url() const = 0;
	virtual QString name() const = 0;
	virtual Device device() const {return Device();}
};

#endif // VIDEO_H
