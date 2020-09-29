#include <QTimer>
#include "timer.h"

Timer::Timer(QObject *parent) : QObject(parent)
{
	_timer = new QTimer();
	_timer->setInterval(1000);
	connect(_timer, &QTimer::timeout, this, &Timer::update);
}

void Timer::start()
{
	_start = QTime::currentTime();
	_timer->start();
}

void Timer::stop()
{
	_timer->stop();
}

void Timer::update()
{
	emit time(_start.msecsTo(QTime::currentTime()));
}
