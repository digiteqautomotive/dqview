#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include "streaminfo.h"

class QComboBox;
class QSpinBox;
class QCheckBox;
class QLineEdit;
class DirSelectWidget;
class StreamTable;
struct Options;

class OptionsDialog : public QDialog
{
public:
	OptionsDialog(Options *options, QWidget *parent = 0);

public slots:
	void accept();

private:
	Options *_options;

	DirSelectWidget *_videoDir;
	DirSelectWidget *_imageDir;
	StreamTable *_streamTable;
	QComboBox *_codec;
	QSpinBox *_bitrate;
	QComboBox *_transform;
	QLineEdit *_aspectRatio;
};

#endif // OPTIONSDIALOG_H
