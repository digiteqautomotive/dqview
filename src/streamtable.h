#ifndef STREAMTABLE_H
#define STREAMTABLE_H

#include <QWidget>
#include "stream.h"

class QTableWidget;
class QPushButton;
class QToolButton;

class StreamTable : public QWidget
{
public:
	StreamTable(QWidget *parent = 0);

	void load(const QList<StreamInfo> &streams);
	void store(QList<StreamInfo> &streams) const;

private slots:
	void add();
	void remove();
	void enableRemove();

private:
	bool findStream(const StreamInfo stream) const;

	QTableWidget *_table;
#ifdef Q_OS_WIN32
	QPushButton *_addButton;
	QPushButton *_removeButton;
#else
	QToolButton *_addButton;
	QToolButton *_removeButton;
#endif
};

#endif // STREAMTABLE_H
