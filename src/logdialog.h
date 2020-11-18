#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>
#include "log.h"

class LogDialog : public QDialog
{
public:
	LogDialog(const Log &log, QWidget *parent);
};

#endif // LOGDIALOG_H
