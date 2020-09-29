#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QTabWidget>
#include <QGroupBox>
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
	QGroupBox *videoBox = new QGroupBox(tr("Video"));
	QFormLayout *videoBoxLayout = new QFormLayout(videoBox);
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


	QTabWidget *tabWidget = new QTabWidget();
	tabWidget->addTab(recordingPage, tr("Recording"));
	tabWidget->addTab(streamsPage, tr("Streams"));

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(tabWidget);
	layout->addWidget(buttonBox);
	show();
}

void OptionsDialog::accept()
{
	_options->videoDir = _videoDir->dir();
	_options->imagesDir = _imageDir->dir();
	_options->streams.clear();
	_streamTable->store(_options->streams);

	QDialog::accept();
}
