#ifndef STREAMDIALOG_H
#define STREAMDIALOG_H

#include <QDialog>
#include "streaminfo.h"

class QLineEdit;
class QSpinBox;
class QComboBox;

class StreamDialog : public QDialog
{
	Q_OBJECT

public:
	StreamDialog(QWidget *parent = 0, const QString &caption = QString());

	StreamInfo stream();

	static StreamInfo getStream(QWidget *parent = 0, const QString &caption = QString());

private:
	QLineEdit *_address;
	QSpinBox *_port;
	QComboBox *_format;
};

#endif // STREAMDIALOG_H
