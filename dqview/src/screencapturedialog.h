#ifndef SCREENCAPTUREDIALOG_H
#define SCREENCAPTUREDIALOG_H

#include <QDialog>

class QSpinBox;
class QCheckBox;
class QGroupBox;
struct Options;

class ScreenCaptureDialog : public QDialog
{
public:
	ScreenCaptureDialog(Options *options, QWidget *parent = 0);

public slots:
	void accept();

private slots:
	void disableRegion(bool disable);

private:
	Options *_options;

	QSpinBox *_width;
	QSpinBox *_height;
	QSpinBox *_top;
	QSpinBox *_left;
	QSpinBox *_fps;
	QCheckBox *_fullScreen;
	QGroupBox *_dimensionsBox;
};

#endif // SCREENCAPTUREDIALOG_H
