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

	virtual QString url() = 0;
	virtual QString name() const = 0;
	virtual Device *device() {return 0;}

	virtual bool isValid() const {return true;}
	virtual bool show() const {return true;}
};

#endif // VIDEO_H
