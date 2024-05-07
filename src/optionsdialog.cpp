#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QTabWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include "dirselectwidget.h"
#include "streamtable.h"
#include "options.h"
#include "optionsdialog.h"


OptionsDialog::OptionsDialog(Options *options, QWidget *parent)
  : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
  _options(options)
{
	setModal(true);
	setWindowTitle(tr("Options"));

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
	  | QDialogButtonBox::Cancel);

	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	_videoDir = new DirSelectWidget();
	_videoDir->setDir(options->videoDir);
	_codec = new QComboBox();
	_codec->addItem("H264", QVariant("h264"));
	_codec->addItem("MPEG-2", QVariant("mp2v"));
	_codec->setCurrentIndex(_codec->findData(QVariant(options->codec)));
	_bitrate = new QSpinBox();
	_bitrate->setMaximum(8192);
	_bitrate->setMinimum(128);
	_bitrate->setSingleStep(100);
	_bitrate->setValue(options->bitrate);
	_bitrate->setSuffix(" kb/s");
	QGroupBox *videoBox = new QGroupBox(tr("Video"));
	QFormLayout *videoBoxLayout = new QFormLayout(videoBox);
	videoBoxLayout->addRow(tr("Codec:"), _codec);
	videoBoxLayout->addRow(tr("Bitrate:"), _bitrate);
	videoBoxLayout->addRow(tr("Directory:"), _videoDir);

	_imageDir = new DirSelectWidget();
	_imageDir->setDir(options->imagesDir);
	QGroupBox *imagesBox = new QGroupBox(tr("Images"));
	QFormLayout *imagesBoxLayout = new QFormLayout(imagesBox);
	imagesBoxLayout->addRow(tr("Directory:"), _imageDir);

	QWidget *recordingPage = new QWidget();
	QVBoxLayout *recordingLayout = new QVBoxLayout(recordingPage);
	recordingLayout->addWidget(videoBox);
	recordingLayout->addWidget(imagesBox);


	_streamTable = new StreamTable();
	_streamTable->load(options->streams);

	QWidget *streamsPage = new QWidget();
	QVBoxLayout *streamsLayout = new QVBoxLayout(streamsPage);
	streamsLayout->addWidget(_streamTable);


	_transform = new QComboBox();
	_transform->addItem("None", QVariant(""));
	_transform->addItem("Flip", QVariant("vflip"));
	_transform->addItem("Rotate 90", QVariant("90"));
	_transform->addItem("Rotate 270", QVariant("270"));
	_transform->setCurrentIndex(_transform->findData(
	  QVariant(options->transform)));

	QLabel *label = new QLabel(tr("Note: Application restart is required for"
	  " the image transformation to take effect!"));
	QFont font = label->font();
	font.setItalic(true);
	label->setFont(font);
	QRegularExpression re("[0-9]+:[0-9]+");
	QRegularExpressionValidator *validator = new QRegularExpressionValidator(re);
	_aspectRatio = new QLineEdit();
	_aspectRatio->setValidator(validator);
	_aspectRatio->setText(options->aspectRatio);


	QWidget *filtersPage = new QWidget();
	QVBoxLayout *filtersLayout = new QVBoxLayout(filtersPage);

	QFormLayout *arLayout = new QFormLayout();
	arLayout->addRow("Force Aspect Ratio:", _aspectRatio);
	arLayout->addRow("Transformation:", _transform);

	filtersLayout->addLayout(arLayout);
	filtersLayout->addStretch();
	filtersLayout->addWidget(label);


	QTabWidget *tabWidget = new QTabWidget();
	tabWidget->addTab(recordingPage, tr("Recording"));
	tabWidget->addTab(streamsPage, tr("Remote Devices"));
	tabWidget->addTab(filtersPage, tr("Video Filters"));

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(tabWidget);
	layout->addWidget(buttonBox);
	show();
}

void OptionsDialog::accept()
{
	_options->bitrate = _bitrate->value();
	_options->codec = _codec->currentData().toString();
	_options->videoDir = _videoDir->dir();
	_options->imagesDir = _imageDir->dir();
	_options->transform = _transform->currentData().toString();
	_options->aspectRatio = _aspectRatio->text();
	_options->streams.clear();
	_streamTable->store(_options->streams);

	QDialog::accept();
}
