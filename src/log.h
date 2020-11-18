#ifndef LOG_H
#define LOG_H

#include <QTime>
#include <QString>
#include <QList>

class LogEntry
{
public:
	LogEntry(const QTime &time, int level, const QString &message)
		: _time(time), _level(level), _message(message) {}

	const QTime &time() const {return _time;}
	int level() const {return _level;}
	const QString &message() const {return _message;}

private:
	QTime _time;
	int _level;
	QString _message;
};

typedef QList<LogEntry> Log;

#endif // LOG_H
