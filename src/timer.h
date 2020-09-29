#ifndef TIMER_H
#define TIMER_H

#include <QObject>
#include <QTime>

class QTimer;

class Timer : public QObject
{
	Q_OBJECT

public:
	Timer(QObject *parent = 0);

public slots:
	void start();
	void stop();

signals:
	void time(int);

private slots:
	void update();

private:
	QTimer *_timer;
	QTime _start;
};

#endif // TIMER_H
