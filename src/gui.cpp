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
#include <QTimer>
#include <QKeyEvent>
#include "timer.h"
#include "videoplayer.h"
#include "camera.h"
#include "stream.h"
#include "streamdialog.h"
#include "optionsdialog.h"
#include "logdialog.h"
#include "gui.h"


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
	setWindowIcon(QIcon(":/app.png"));
	setWindowTitle(APP_NAME);

	readSettings();

	_player = new VideoPlayer(_options.flip);
	_player->setImageDir(_options.imagesDir);
	_player->setVideoDir(_options.videoDir);
	_player->setCodec(_options.codec);
	_player->setBitrate(_options.bitrate);
	connect(_player, &VideoPlayer::error, this, &GUI::streamError);
	connect(_player, &VideoPlayer::stateChanged, this, &GUI::stateChanged);

	createActions();
	createMenus();
	createToolbars();
	createStatusBar();

	setCentralWidget(_player);
	restoreGeometry(_options.windowGeometry);
	restoreState(_options.windowState);

	QList<QAction*> devices(_deviceActionGroup->actions());
	if (!devices.isEmpty())
		devices.first()->trigger();
}

QList<QAction*> GUI::cameraActions()
{
	QList<CameraInfo> cameras = CameraInfo::availableCameras();
	QList<QAction*> list;

	QSignalMapper *signalMapper = new QSignalMapper(this);
	connect(signalMapper, QOverload<QObject *>::of(&QSignalMapper::mapped),
	  this, &GUI::openDevice);

	qSort(cameras);

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
	_resizeActionGroup = new QActionGroup(this);
	_resizeActionGroup->setExclusive(true);

	_openStreamAction = new QAction(tr("Open Remote Device..."), this);
	connect(_openStreamAction, &QAction::triggered, this,
	  QOverload<>::of(&GUI::openStream));
	_exitAction = new QAction(tr("Quit"), this);
	connect(_exitAction, &QAction::triggered, this, &GUI::close);

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

	_resizeVideoAction = new QAction(tr("Resize Video To Window"));
	_resizeVideoAction->setCheckable(true);
	_resizeVideoAction->setChecked(!(_options.resize || _options.fullScreen));
	_resizeVideoAction->setActionGroup(_resizeActionGroup);
	_resizeWindowAction = new QAction(tr("Resize Window To Video"));
	_resizeWindowAction->setCheckable(true);
	_resizeWindowAction->setChecked(_options.resize);
	_resizeWindowAction->setActionGroup(_resizeActionGroup);
	_fullScreenAction = new QAction(tr("Fullscreen Mode"));
	_fullScreenAction->setCheckable(true);
	_fullScreenAction->setChecked(_options.fullScreen);
	_fullScreenAction->setActionGroup(_resizeActionGroup);

	_optionsAction = new QAction(tr("Options..."));
	connect(_optionsAction, &QAction::triggered, this, &GUI::openOptions);


	_showLogAction = new QAction("Show Log File...");
	connect(_showLogAction, &QAction::triggered, this, &GUI::showLog);
	_aboutAction = new QAction(QIcon(":/app.png"), tr("About MGB Viewer"), this);
	connect(_aboutAction, &QAction::triggered, this, &GUI::about);
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
	settingsMenu->addAction(_resizeVideoAction);
	settingsMenu->addAction(_resizeWindowAction);
	settingsMenu->addAction(_fullScreenAction);
	settingsMenu->addSeparator();
	settingsMenu->addAction(_optionsAction);

	QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(_showLogAction);
	helpMenu->addAction(_aboutAction);
}

void GUI::createToolbars()
{
	_videoToolBar = addToolBar(tr("Video"));
	_videoToolBar->setObjectName("VideoToolBar");
	_videoToolBar->addAction(_playAction);
	_videoToolBar->addSeparator();
	_videoToolBar->addAction(_recordAction);
	_videoToolBar->addAction(_screenshotAction);
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
		startStreaming();
		if (_recordAction->isChecked())
			_player->startStreamingAndRecording();
		else
			_player->startStreaming();
	} else {
		stopStreaming();
		_player->stopStreaming();
	}
}

void GUI::startStreaming()
{
	_recordAction->setEnabled(false);
	_playAction->setEnabled(false);
	_deviceActionGroup->setEnabled(false);
	_resizeActionGroup->setEnabled(false);
	_openStreamAction->setEnabled(false);
}

void GUI::stopStreaming()
{
	_playAction->setEnabled(false);
	_screenshotAction->setEnabled(false);
	showFullScreen(false);
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

void GUI::stateChanged(bool playing)
{
	if (playing) {
		if (_recordAction->isChecked())
			startRecording();
		if (!_recordAction->isChecked())
			_screenshotAction->setEnabled(true);
		 QTimer::singleShot(100, this, SLOT(videoLoaded()));
	} else {
		if (_recordAction->isChecked())
			stopRecording();
		_resolutionLabel->setText(QString());
		_recordAction->setEnabled(true);
		_deviceActionGroup->setEnabled(true);
		_resizeActionGroup->setEnabled(true);
		_openStreamAction->setEnabled(true);
		/* The playback can be terminated by libVLC itself, eg. when no network
		   data has arrived in 10s */
		_playAction->setChecked(false);
	}

	_playAction->setEnabled(true);
}

void GUI::videoLoaded()
{
	if (_player->resolution().isNull()) {
		QTimer::singleShot(100, this, SLOT(videoLoaded()));
		return;
	}

	if (_fullScreenAction->isChecked()) {
		showFullScreen(true);
	} else if (_resizeWindowAction->isChecked()) {
		QSize diff(window()->size() - _player->size());
		window()->resize(_player->resolution() + diff);
	}

	_resolutionLabel->setText(QString("%1x%2").arg(
	  QString::number(_player->resolution().width()),
	  QString::number(_player->resolution().height())));
}

void GUI::openDevice(QObject *device)
{
	Video *video = qobject_cast<Video *>(device);
	_player->setVideo(video);
	_deviceNameLabel->setText(video->name());

	_playAction->setEnabled(true);
}

void GUI::openStream()
{
	StreamInfo info = StreamDialog::getStream(this, tr("Open Remote Device"));
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
	_player->setCodec(_options.codec);
	_player->setBitrate(_options.bitrate);
}

void GUI::about()
{
	QMessageBox msgBox(this);
	QUrl homepage(APP_HOMEPAGE);

	msgBox.setWindowTitle(tr("About MGB Viewer"));
	msgBox.setText("<h2>" + QString(APP_NAME) + "</h2><p><p>" + tr("Version %1")
	  .arg(QString(APP_VERSION) + " (Qt " + QT_VERSION_STR + ", libVLC "
	  + libvlc_get_version() + ")") + "</p>");
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

void GUI::showLog()
{
	LogDialog dialog(_player->log(), this);
	dialog.exec();
}

void GUI::showToolbars(bool show)
{
	if (show) {
		Q_ASSERT(!_windowStates.isEmpty());
		restoreState(_windowStates.last());
		_windowStates.pop_back();
	} else {
		_windowStates.append(saveState());
		removeToolBar(_videoToolBar);
	}
}

void GUI::showFullScreen(bool show)
{
	if (isFullScreen() == show)
		return;

	if (show) {
		statusBar()->hide();
		menuBar()->hide();
		showToolbars(false);
		QMainWindow::showFullScreen();
	} else {
		statusBar()->show();
		menuBar()->show();
		showToolbars(true);
		showNormal();
	}
}

void GUI::closeEvent(QCloseEvent *event)
{
	writeSettings();
	QMainWindow::closeEvent(event);
}

void GUI::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Space:
			if (_playAction->isEnabled())
				_playAction->trigger();
			break;
	}

	QMainWindow::keyPressEvent(event);
}

void GUI::readSettings()
{
	qRegisterMetaTypeStreamOperators<StreamInfo>("Stream");

	QSettings settings(COMPANY_NAME, APP_NAME);

	settings.beginGroup("Window");
	_options.windowGeometry = settings.value("Geometry").toByteArray();
	_options.windowState = settings.value("WindowState").toByteArray();
	_options.resize = settings.value("Resize").toBool();
	_options.fullScreen = settings.value("FullScreen").toBool();
	settings.endGroup();

	settings.beginGroup("Video");
	_options.flip = settings.value("Flip").toBool();
	settings.endGroup();

	settings.beginGroup("Recording");
	_options.codec = settings.value("Codec", "h264").toString();
	_options.bitrate = settings.value("Bitrate", 1800).toUInt();
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
	settings.setValue("Resize", _resizeWindowAction->isChecked());
	settings.setValue("FullScreen", _fullScreenAction->isChecked());
	settings.endGroup();

	settings.beginGroup("Video");
	settings.setValue("Flip", _options.flip);
	settings.endGroup();

	settings.beginGroup("Recording");
	settings.setValue("Codec", _options.codec);
	settings.setValue("Bitrate", _options.bitrate);
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
