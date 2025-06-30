#ifndef CONFIGCOPYDIALOG_H
#define CONFIGCOPYDIALOG_H

#include <QDialog>

class QComboBox;
class OutputConfigDialog;

class ConfigCopyDialog : public QDialog
{
public:
	ConfigCopyDialog(OutputConfigDialog *config, QWidget *parent = 0);

public slots:
	void accept();

private:
	OutputConfigDialog *_config;
	QComboBox *_copySrc;
};

#endif // CONFIGCOPYDIALOG_H
