#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QSplitter>
#include <QFileDialog>
#include <QSignalMapper>
#include <QIcon>
#include <QToolBar>
#include <QLabel>
#include <QStatusBar>
#include <QMessageBox>
#include <QSysInfo>
#include "timer.h"
#include "videoplayer.h"
#include "camera.h"
#include "stream.h"
#include "streamdialog.h"
#include "optionsdialog.h"
#include "gui.h"

#include <QDebug>

#define COMPANY_NAME "Digiteq Automotive"
#define APP_NAME "MGB Viewer"
#define APP_HOMEPAGE "https://www.digiteqautomotive.com"


static QString timeSpan(int time)
{
	unsigned h, m, s;

	h = time / 3600;
	m = (time - (h * 3600)) / 60;
	s = time - (h * 3600) - (m * 60);

	if (h)
		return QString("%1:%2:%3").arg(h, 2, 10, QChar('0'))
		  .arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
	else
		return QString("%1:%2").arg(m, 2, 10, QChar('0'))
		  .arg(s, 2, 10, QChar('0'));
}


GUI::GUI()
{
	readSettings();

	_player = new VideoPlayer();
	_player->setImageDir(_options.imagesDir);
	_player->setVideoDir(_options.videoDir);
	connect(_player, &VideoPlayer::error, this, &GUI::streamError);
	connect(_player, &VideoPlayer::stateChanged, this, &GUI::stateChange);
	connect(_player, &VideoPlayer::recordingStateChanged, this,
	  &GUI::recordingStateChange);
	connect(_player, &VideoPlayer::videoOutputReady, this,
	  &GUI::resolutionReady);

	createActions();
	createMenus();
	createToolbars();
	createStatusBar();

	setCentralWidget(_player);

	setWindowIcon(QIcon(":/app.png"));
	setWindowTitle(APP_NAME);
}

QList<QAction*> GUI::cameraActions()
{
	const QList<CameraInfo> cameras = CameraInfo::availableCameras();
	QList<QAction*> list;

	QSignalMapper *signalMapper = new QSignalMapper(this);
	connect(signalMapper, QOverload<QObject *>::of(&QSignalMapper::mapped),
	  this, &GUI::openDevice);

	for (int i = 0; i < cameras.size(); i++) {
		QAction *a = new QAction(cameras.at(i).description(), this);
		a->setActionGroup(_deviceActionGroup);
		a->setCheckable(true);
		Camera *cam = new Camera(cameras.at(i), this);
		signalMapper->setMapping(a, cam);
		connect(a, &QAction::triggered, signalMapper,
		  QOverload<>::of(&QSignalMapper::map));
		list.append(a);
	}

	return list;
}

QAction *GUI::streamAction(Stream *stream)
{
	QAction *a = new QAction(stream->name());
	a->setActionGroup(_deviceActionGroup);
	a->setCheckable(true);
	a->setData(QVariant::fromValue(stream->info()));
	_streamSignalMapper->setMapping(a, stream);
	connect(a, &QAction::triggered, _streamSignalMapper,
	  QOverload<>::of(&QSignalMapper::map));

	return a;
}

QList<QAction*> GUI::streamActions()
{
	_streamSignalMapper = new QSignalMapper(this);
	connect(_streamSignalMapper, QOverload<QObject *>::of(
	  &QSignalMapper::mapped), this, QOverload<QObject *>::of(&GUI::openDevice));
	QList<QAction*> list;

	for (int i = 0; i < _options.streams.count(); i++) {
		Stream *stream = new Stream(_options.streams.at(i), this);
		list.append(streamAction(stream));
	}

	return list;
}

void GUI::createActions()
{
	_deviceActionGroup = new QActionGroup(this);
	_deviceActionGroup->setExclusive(true);

	_openStreamAction = new QAction(tr("Open Stream..."), this);
	connect(_openStreamAction, &QAction::triggered, this,
	  QOverload<>::of(&GUI::openStream));

	_playAction = new QAction(QIcon(":/play.png"), tr("Play"));
	_playAction->setCheckable(true);
	_playAction->setEnabled(false);
	connect(_playAction, &QAction::triggered, this, &GUI::play);

	_recordAction = new QAction(QIcon(":/record.png"), tr("Record Video"));
	_recordAction->setCheckable(true);
	_screenshotAction = new QAction(QIcon(":/screenshot.png"),
	  tr("Capture Screenshot"));
	_screenshotAction->setEnabled(false);
	connect(_screenshotAction, &QAction::triggered, _player,
	  &VideoPlayer::captureImage);

	_resizeAction = new QAction(tr("Resize Window To Video"));
	_resizeAction->setCheckable(true);
	_resizeAction->setChecked(_options.resize);
	_optionsAction = new QAction(tr("Options..."));
	connect(_optionsAction, &QAction::triggered, this, &GUI::openOptions);

	_aboutAction = new QAction(QIcon(":/app.png"), tr("About MGB Viewer"), this);
	connect(_aboutAction, &QAction::triggered, this, &GUI::about);
	_exitAction = new QAction(tr("Quit"), this);
	connect(_exitAction, &QAction::triggered, this, &GUI::close);
}

void GUI::createMenus()
{
	_deviceMenu = menuBar()->addMenu(tr("&Device"));
	_deviceMenu->addActions(cameraActions());
	_deviceMenu->addActions(streamActions());
	_deviceSeparator = _deviceMenu->addSeparator();
	_deviceMenu->addAction(_openStreamAction);
	_deviceMenu->addSeparator();
	_deviceMenu->addAction(_exitAction);

	QMenu *videoMenu = menuBar()->addMenu(tr("&Video"));
	videoMenu->addAction(_playAction);
	videoMenu->addSeparator();
	videoMenu->addAction(_recordAction);
	videoMenu->addAction(_screenshotAction);

	QMenu *settingsMenu = menuBar()->addMenu(tr("&Settings"));
	settingsMenu->addAction(_resizeAction);
	settingsMenu->addAction(_optionsAction);

	QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(_aboutAction);
}

void GUI::createToolbars()
{
	QToolBar *videoToolBar = addToolBar(tr("Video"));
	videoToolBar->addAction(_playAction);
	videoToolBar->addSeparator();
	videoToolBar->addAction(_recordAction);
	videoToolBar->addAction(_screenshotAction);
}

void GUI::createStatusBar()
{
	_deviceNameLabel = new QLabel();
	_resolutionLabel = new QLabel();
	_recordFileLabel = new QLabel();
	_recordTimeLabel = new QLabel();
	_recordTimeLabel->setAlignment(Qt::AlignHCenter);

	statusBar()->addPermanentWidget(_deviceNameLabel);
	statusBar()->addPermanentWidget(_resolutionLabel);
	statusBar()->addPermanentWidget(new QLabel(), 10);
	statusBar()->addPermanentWidget(_recordFileLabel);
	statusBar()->addPermanentWidget(_recordTimeLabel);
	statusBar()->setSizeGripEnabled(false);

	_deviceNameLabel->setText(tr("No Device Open"));

	_recordTimer = new Timer(this);
	connect(_recordTimer, &Timer::time, this, &GUI::updateTimer);
}

void GUI::updateTimer(int time)
{
	_recordTimeLabel->setText(timeSpan(time / 1000));
}

void GUI::play(bool enable)
{
	if (enable) {
		_recordAction->setEnabled(false);
		if (_recordAction->isChecked())
			startStreamingAndRecording();
		else
			startStreaming();
	} else {
		stopStreaming();
	}
}

void GUI::startStreaming()
{
	_playAction->setEnabled(false);
	_deviceActionGroup->setEnabled(false);
	_openStreamAction->setEnabled(false);

	_player->startStreaming();
}

void GUI::startStreamingAndRecording()
{
	_playAction->setEnabled(false);
	_deviceActionGroup->setEnabled(false);
	_openStreamAction->setEnabled(false);

	_player->startStreamingAndRecording();
}

void GUI::stopStreaming()
{
	_playAction->setEnabled(false);
	_screenshotAction->setEnabled(false);

	_player->stopStreaming();
}

void GUI::startRecording()
{
	_recordFileLabel->setText(_player->recordFile());
	_recordTimeLabel->setText(timeSpan(0));
	_recordTimer->start();
}

void GUI::stopRecording()
{
	_recordTimer->stop();
	_recordFileLabel->setText(QString());
	_recordTimeLabel->setText(QString());
}

void GUI::streamError(const QString &error)
{
	stopRecording();
	stopStreaming();

	QMessageBox::critical(this, tr("Stream Error"), error);

	/* Force the play action state as in case of an error, stateChange() may not
	   be called. */
	_playAction->setChecked(false);
	_playAction->setEnabled(true);
	_deviceActionGroup->setEnabled(true);
	_openStreamAction->setEnabled(true);
}

void GUI::stateChange(bool playing)
{
	qDebug() << "stateChange" << playing;

	if (playing) {
		_screenshotAction->setEnabled(true);
	} else {
		_deviceActionGroup->setEnabled(true);
		_openStreamAction->setEnabled(true);
		_resolutionLabel->setText(QString());
		_recordAction->setEnabled(true);
	}

	_playAction->setEnabled(true);
}

#include <unistd.h>
void GUI::resolutionReady()
{
	if (_resizeAction->isChecked()) {
		QSize diff(window()->size() - _player->size());
		window()->resize(_player->resolution() + diff);
	}

	_resolutionLabel->setText(QString("%1x%2").arg(
	  QString::number(_player->resolution().width()),
	  QString::number(_player->resolution().height())));
}

void GUI::recordingStateChange(bool recording)
{
	if (recording)
		startRecording();
	else
		stopRecording();
}

void GUI::openDevice(QObject *device)
{
	Video *video = qobject_cast<Video *>(device);
	_player->setVideo(video);

	_deviceNameLabel->setText(video->name());
	_playAction->setEnabled(true);

	_playAction->trigger();
}

void GUI::openStream()
{
	StreamInfo info = StreamDialog::getStream(this, tr("Open Video Stream"));
	if (info.isNull())
		return;

	Stream *stream = new Stream(info, this);
	QAction *a = streamAction(stream);
	a->setChecked(true);
	_deviceMenu->insertAction(_deviceSeparator, a);

	openDevice(stream);
}

bool GUI::findStream(const StreamInfo &stream) const
{
	QList<QAction *> actions = _deviceActionGroup->actions();

	for (int j = 0; j < actions.size(); j++) {
		const StreamInfo &as = actions.at(j)->data().value<StreamInfo>();
		if (as == stream)
			return true;
	}

	return false;
}

void GUI::openOptions()
{
	OptionsDialog dialog(&_options, this);
	dialog.exec();

	for (int i = 0; i < _options.streams.count(); i++) {
		const StreamInfo &stream = _options.streams.at(i);

		if (!findStream(stream)) {
			QAction *a = streamAction(new Stream(stream, this));
			_deviceMenu->insertAction(_deviceSeparator, a);
		}
	}

	_player->setImageDir(_options.imagesDir);
	_player->setVideoDir(_options.videoDir);
}

void GUI::about()
{
	QMessageBox msgBox(this);
	QUrl homepage(APP_HOMEPAGE);

	msgBox.setWindowTitle(tr("About MGB Viewer"));
	msgBox.setText("<h2>" + QString(APP_NAME) + "</h2><p><p>" + tr("Version %1")
	  .arg(QString(APP_VERSION) + " (" + QSysInfo::buildCpuArchitecture()
	  + ", Qt " + QT_VERSION_STR + ")") + "</p>");
	msgBox.setInformativeText("<table width=\"300\"><tr><td>"
	  + tr("MGB Viewer is distributed under the terms of the GNU General Public "
	  "License version 3. For more info about MGB Viewer visit the project "
	  "homepage at %1.").arg("<a href=\"" + homepage.toString() + "\">"
	  + homepage.toString(QUrl::RemoveScheme).mid(2) + "</a>")
	  + "</td></tr></table>");

	QIcon icon = msgBox.windowIcon();
	QSize size = icon.actualSize(QSize(64, 64));
	msgBox.setIconPixmap(icon.pixmap(size));

	msgBox.exec();
}

void GUI::closeEvent(QCloseEvent *event)
{
	stopRecording();
	stopStreaming();

	writeSettings();

	QMainWindow::closeEvent(event);
}

void GUI::readSettings()
{
	qRegisterMetaTypeStreamOperators<StreamInfo>("Stream");

	QSettings settings(COMPANY_NAME, APP_NAME);

	settings.beginGroup("Window");
	restoreGeometry(settings.value("Geometry").toByteArray());
	restoreState(settings.value("WindowState").toByteArray());
	_options.resize = settings.value("Resize").toBool();
	settings.endGroup();

	settings.beginGroup("Recording");
	_options.videoDir = settings.value("VideoDir", QDir::homePath()).toString();
	_options.imagesDir = settings.value("ImagesDir", QDir::homePath()).toString();
	settings.endGroup();

	settings.beginGroup("Streams");
	int size = settings.beginReadArray("Streams");
	for (int i = 0; i < size; i++) {
		settings.setArrayIndex(i);
		_options.streams.append(settings.value("Stream").value<StreamInfo>());
	}
	settings.endArray();
	settings.endGroup();
}

void GUI::writeSettings()
{
	QSettings settings(COMPANY_NAME, APP_NAME);

	settings.beginGroup("Window");
	settings.setValue("Geometry", saveGeometry());
	settings.setValue("WindowState", saveState());
	settings.setValue("Resize", _resizeAction->isChecked());
	settings.endGroup();

	settings.beginGroup("Recording");
	settings.setValue("VideoDir", _options.videoDir);
	settings.setValue("ImagesDir", _options.imagesDir);
	settings.endGroup();

	settings.beginGroup("Streams");
	settings.beginWriteArray("Streams");
	for (int i = 0; i < _options.streams.count(); i++) {
		settings.setArrayIndex(i);
		settings.setValue("Stream", QVariant::fromValue(_options.streams.at(i)));
	}
	settings.endArray();
	settings.endGroup();
}
