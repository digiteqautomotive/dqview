#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include "streaminfo.h"

class QTableWidget;
class QToolButton;
class DirSelectWidget;
class StreamTable;
class Options;


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
};

#endif // OPTIONSDIALOG_H
