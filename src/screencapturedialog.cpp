#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSpinBox>
#include <QCheckBox>
#include "options.h"
#include "screencapturedialog.h"

ScreenCaptureDialog::ScreenCaptureDialog(Options *options, QWidget *parent)
  : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
  _options(options)
{
	setModal(true);
	setWindowTitle(tr("Screen Capture"));
	setMinimumWidth(300);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
	  | QDialogButtonBox::Cancel);

	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	_width = new QSpinBox();
	_width->setMaximum(4096);
	_width->setValue(_options->screenWidth);
	_height = new QSpinBox();
	_height->setMaximum(4096);
	_height->setValue(_options->screenHeight);
	_top = new QSpinBox();
	_top->setMaximum(4096);
	_top->setValue(_options->screenTop);
	_left = new QSpinBox();
	_left->setMaximum(4096);
	_left->setValue(_options->screenLeft);

	_fullScreen = new QCheckBox(tr("Capture whole screen"));
	_fullScreen->setChecked(_options->screenFull);
	connect(_fullScreen, &QCheckBox::toggled, this,
	  &ScreenCaptureDialog::disableRegion);

	_fps = new QSpinBox();
	_fps->setMinimum(25);
	_fps->setMaximum(60);
	_fps->setValue(_options->screenFPS);

	_dimensionsBox = new QGroupBox(tr("Screen Region"));
	_dimensionsBox->setEnabled(!_options->screenFull);
	QFormLayout *dimensionsLayout = new QFormLayout(_dimensionsBox);
	dimensionsLayout->addRow(tr("Width:"), _width);
	dimensionsLayout->addRow(tr("Height:"), _height);
	dimensionsLayout->addRow(tr("Top:"), _top);
	dimensionsLayout->addRow(tr("Left:"), _left);

	QGroupBox *optionsBox = new QGroupBox(tr("Options"));
	QFormLayout *optionsLayout = new QFormLayout(optionsBox);
	optionsLayout->addRow(tr("Frame Rate:"), _fps);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(optionsBox);
	layout->addWidget(_dimensionsBox);
	layout->addWidget(_fullScreen);
	layout->addWidget(buttonBox);
	show();
}

void ScreenCaptureDialog::disableRegion(bool disable)
{
	_dimensionsBox->setEnabled(!disable);
}

void ScreenCaptureDialog::accept()
{
	_options->screenWidth = _width->value();
	_options->screenHeight = _height->value();
	_options->screenTop = _top->value();
	_options->screenLeft = _left->value();
	_options->screenFull = _fullScreen->isChecked();
	_options->screenFPS = _fps->value();

	QDialog::accept();
}
