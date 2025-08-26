#include <QAction>
#include <QActionGroup>
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
#include "screencapturedialog.h"
#include "logdialog.h"
#include "deviceconfigdialog.h"
#include "gui.h"


#define COMPANY_NAME "Digiteq Automotive"
#define APP_NAME "DQ Viewer"
#define APP_HOMEPAGE "https://github.com/digiteqautomotive/dqview"

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

static bool cmp(const DeviceInfo *a, const DeviceInfo *b)
{
	return *a < *b;
}

GUI::GUI() : _video(0)
{
	setWindowIcon(QIcon(":/app.png"));
	setWindowTitle(APP_NAME);

	readSettings();

	_player = new VideoPlayer(_options.transform);
	_player->setImageDir(_options.imagesDir);
	_player->setVideoDir(_options.videoDir);
	_player->setCodec(_options.codec);
	_player->setBitrate(_options.bitrate);
	_player->setAspectRatio(_options.aspectRatio);
	connect(_player, &VideoPlayer::error, this, &GUI::streamError);
	connect(_player, &VideoPlayer::stateChanged, this, &GUI::stateChanged);

	createActions();
	createMenus();
	createToolbars();
	createStatusBar();

	setCentralWidget(_player);
	restoreGeometry(_options.windowGeometry);
	restoreState(_options.windowState);
	if (isFullScreen()) {
		statusBar()->hide();
		menuBar()->hide();
		showToolbars(false);
	}

	QList<QAction*> devices(_deviceActionGroup->actions());
	if (!devices.isEmpty())
		devices.first()->trigger();

	_outputFile = new VideoFile(this);
	_screenCapture = new ScreenCapture(this);
	_output = _outputFile;
}

QList<QAction*> GUI::deviceActions(Device::Type type)
{
	QList<DeviceInfo*> devices = (type == Device::Input)
	  ? DeviceInfo::inputDevices() : DeviceInfo::outputDevices();
	QList<QAction*> list;

	QSignalMapper *signalMapper = new QSignalMapper(this);
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
	connect(signalMapper, QOverload<QObject *>::of(&QSignalMapper::mapped),
	  this, &GUI::openDevice);
#else
	connect(signalMapper, &QSignalMapper::mappedObject, this, &GUI::openDevice);
#endif

	std::sort(devices.begin(), devices.end(), cmp);

	for (int i = 0; i < devices.size(); i++) {
		QAction *a = new QAction(devices.at(i)->name(), this);
		a->setActionGroup(_deviceActionGroup);
		a->setCheckable(true);
		Camera *cam = new Camera(devices.at(i), this);
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

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
	connect(_streamSignalMapper, QOverload<QObject *>::of(&QSignalMapper::mapped),
	  this, QOverload<QObject *>::of(&GUI::openDevice));
#else
	connect(_streamSignalMapper, &QSignalMapper::mappedObject, this,
	  QOverload<QObject *>::of(&GUI::openDevice));
#endif
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
	_configureDeviceAction = new QAction(tr("Configure Device..."), this);
	_configureDeviceAction->setEnabled(false);
	connect(_configureDeviceAction, &QAction::triggered, this,
	  &GUI::configureDevice);
	_exitAction = new QAction(tr("Quit"), this);
	connect(_exitAction, &QAction::triggered, this, &GUI::close);

	_playAction = new QAction(QIcon(":/play.png"), tr("Play"));
	_playAction->setCheckable(true);
	_playAction->setEnabled(false);
	connect(_playAction, &QAction::triggered, this, &GUI::play);

	_recordAction = new QAction(QIcon(":/record.png"), tr("Record Video"));
	_recordAction->setCheckable(true);
	_recordAction->setEnabled(false);
	_screenshotAction = new QAction(QIcon(":/screenshot.png"),
	  tr("Capture Screenshot"));
	_screenshotAction->setEnabled(false);
	connect(_screenshotAction, &QAction::triggered, _player,
	  &VideoPlayer::captureImage);

	_selectOutputFileAction = new QAction(tr("Open Media File..."));
	_selectOutputFileAction->setEnabled(false);
	connect(_selectOutputFileAction, &QAction::triggered, this,
	  &GUI::selectOutputFile);
	_selectOutputDesktopAction = new QAction(tr("Capture Desktop..."));
	_selectOutputDesktopAction->setEnabled(false);
	connect(_selectOutputDesktopAction, &QAction::triggered, this,
	  &GUI::selectOutputDesktop);
	_loopAction = new QAction(QIcon(":/loop.png"), tr("Loop video"));
	_loopAction->setCheckable(true);
	_loopAction->setEnabled(false);

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
	_aboutAction = new QAction(QIcon(":/app.png"), tr("About DQ Viewer"), this);
	connect(_aboutAction, &QAction::triggered, this, &GUI::about);
}

void GUI::createMenus()
{
	QList<QAction*> inputDevices(deviceActions(Device::Input));
	QList<QAction*> outputDevices(deviceActions(Device::Output));

	_deviceMenu = menuBar()->addMenu(tr("&Device"));
	if (!outputDevices.isEmpty())
		_deviceMenu->addSection(tr("Input Devices"));
	_deviceMenu->addActions(inputDevices);
	_deviceMenu->addActions(streamActions());
	if (!outputDevices.isEmpty()) {
		_deviceSeparator = _deviceMenu->addSection(tr("Output Devices"));
		_deviceMenu->addActions(outputDevices);
		_deviceMenu->addSeparator();
	} else
		_deviceSeparator = _deviceMenu->addSeparator();
	_deviceMenu->addAction(_openStreamAction);
	_deviceMenu->addAction(_configureDeviceAction);
	_deviceMenu->addSeparator();
	_deviceMenu->addAction(_exitAction);

	QMenu *videoMenu = menuBar()->addMenu(tr("&Video"));
	videoMenu->addAction(_playAction);
	videoMenu->addSeparator();
	videoMenu->addAction(_recordAction);
	videoMenu->addAction(_screenshotAction);
	videoMenu->addSeparator();
	videoMenu->addAction(_selectOutputDesktopAction);
	videoMenu->addAction(_selectOutputFileAction);
	videoMenu->addAction(_loopAction);

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
	_videoToolBar->addSeparator();
	_videoToolBar->addAction(_loopAction);
}

void GUI::createStatusBar()
{
	_videoSourceLabel = new QLabel();
	_resolutionLabel = new QLabel();
	_recordFileLabel = new QLabel();
	_recordTimeLabel = new QLabel();
	_recordTimeLabel->setAlignment(Qt::AlignHCenter);

	statusBar()->addPermanentWidget(_videoSourceLabel);
	statusBar()->addPermanentWidget(_resolutionLabel);
	statusBar()->addPermanentWidget(new QLabel(), 10);
	statusBar()->addPermanentWidget(_recordFileLabel);
	statusBar()->addPermanentWidget(_recordTimeLabel);
	statusBar()->setSizeGripEnabled(false);

	_videoSourceLabel->setText(tr("No Device Open"));

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
		if (_video->device()->type() == Device::Output)
			_player->startStreamingOut();
		else
			_player->startStreaming(_recordAction->isChecked());
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
	_selectOutputFileAction->setEnabled(false);
	_selectOutputDesktopAction->setEnabled(false);
}

void GUI::stopStreaming()
{
	_playAction->setEnabled(false);
	_screenshotAction->setEnabled(false);
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
	_selectOutputFileAction->setEnabled(true);
	_selectOutputDesktopAction->setEnabled(true);
}

void GUI::stateChanged(bool playing)
{
	if (playing) {
		if (_recordAction->isChecked())
			startRecording();
		if (!_recordAction->isChecked() && _video->device()->type() == Device::Input)
			_screenshotAction->setEnabled(true);
		 QTimer::singleShot(100, this, SLOT(videoLoaded()));
	} else {
		if (_playAction->isChecked() && _loopAction->isChecked()) {
			play(true);
			return;
		}

		if (_recordAction->isChecked())
			stopRecording();
		_resolutionLabel->setText(QString());
		_recordAction->setEnabled(_video->device()->type() == Device::Input);
		_loopAction->setEnabled(_video->device()->type() == Device::Output);
		_selectOutputFileAction->setEnabled(_video->device()->type() == Device::Output);
		_selectOutputDesktopAction->setEnabled(_video->device()->type() == Device::Output);
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

	if (_player->video()->show()) {
		if (_fullScreenAction->isChecked()) {
			showFullScreen(true);
		} else if (_resizeWindowAction->isChecked()) {
			QSize diff(window()->size() - _player->size());
			window()->resize(_player->resolution() + diff);
		}
	}

	_resolutionLabel->setText(QString("%1x%2").arg(
	  QString::number(_player->resolution().width()),
	  QString::number(_player->resolution().height())));
}

void GUI::openDevice(QObject *device)
{
	Video *video = qobject_cast<Video *>(device);
	_video = video;

	if (_video->device()->type() == Device::Input) {
		_player->setVideo(video);
		_videoSourceLabel->setText(video->name());
		_playAction->setEnabled(true);
		_recordAction->setEnabled(true);
		_loopAction->setEnabled(false);
		_selectOutputFileAction->setEnabled(false);
		_selectOutputDesktopAction->setEnabled(false);
	} else if (_video->device()->type() == Device::Output) {
		_player->setDisplay(_video->device());
		_player->setVideo(_output);
		_videoSourceLabel->setText(_output->isValid()
		  ? _output->name() : "No video file selected");
		_playAction->setEnabled(_output->isValid());
		_recordAction->setEnabled(false);
		_loopAction->setEnabled(true);
		_selectOutputFileAction->setEnabled(true);
		_selectOutputDesktopAction->setEnabled(true);
	}

	_configureDeviceAction->setEnabled(_video->device()->isValid());
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

void GUI::selectOutputFile()
{
	QString file(QFileDialog::getOpenFileName(this, tr("Open file"), QString(),
	  tr("Video files") + " (*.avi *.mkv *.mp4 *.mpeg *.mov *.mpg *.ts *.webm *.wmv);;"
	  + tr("Image files") + " (*.jpg *.jpeg *.png);;"
	  + tr("All files") + " (*)"));

	if (!file.isEmpty()) {
		_output = _outputFile;
		_player->setVideo(_output);

		_outputFile->setFile(file);
		if (_video->device()->type() == Device::Output) {
			_videoSourceLabel->setText(_output->name());
			if (_video->device()->isValid())
				_playAction->setEnabled(true);
		}
	}
}

void GUI::selectOutputDesktop()
{
	ScreenCaptureDialog dialog(&_options, this);
	dialog.exec();

	_output = _screenCapture;
	_player->setVideo(_output);

	if (_options.screenFull)
		_screenCapture->captureFullscreen(_options.screenFPS);
	else
		_screenCapture->captureRegion(_options.screenFPS,
		  QSize(_options.screenWidth, _options.screenHeight),
		  QPoint(_options.screenLeft, _options.screenTop));

	if (_video->device()->type() == Device::Output) {
		_videoSourceLabel->setText(_output->name());
		if (_video->device()->isValid())
			_playAction->setEnabled(true);
	}
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
	_player->setAspectRatio(_options.aspectRatio);
}

void GUI::configureDevice()
{
	if (_video->device()->type() == Device::Output) {
		OutputConfigDialog dialog(_video->device(), this);
		dialog.exec();
	} else {
		InputConfigDialog dialog(_video->device(), this);
		dialog.exec();
	}
}

void GUI::about()
{
	QMessageBox msgBox(this);
	QUrl homepage(APP_HOMEPAGE);

	msgBox.setWindowTitle(tr("About DQ Viewer"));
	msgBox.setText("<h2>" + QString(APP_NAME) + "</h2><p><p>" + tr("Version %1")
	  .arg(QString(APP_VERSION) + " (Qt " + QT_VERSION_STR + ", libVLC "
	  + libvlc_get_version() + ")") + "</p>");
	msgBox.setInformativeText("<table width=\"300\"><tr><td>"
	  + tr("DQ Viewer is distributed under the terms of the GNU General Public "
	  "License version 3. For more info about DQ Viewer visit the project "
	  "homepage at %1.").arg("<a href=\"" + homepage.toString() + "\">"
	  + homepage.host() + "</a>") + "</td></tr></table>");

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
	if (_playAction->isChecked())
		_playAction->trigger();

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
		case Qt::Key_Escape:
			showFullScreen(false);
			break;
	}

	QMainWindow::keyPressEvent(event);
}

void GUI::readSettings()
{
	QSettings settings(COMPANY_NAME, APP_NAME);

	settings.beginGroup("Window");
	_options.windowGeometry = settings.value("Geometry").toByteArray();
	_options.windowState = settings.value("WindowState").toByteArray();
	_options.resize = settings.value("Resize").toBool();
	_options.fullScreen = settings.value("FullScreen").toBool();
	settings.endGroup();

	settings.beginGroup("Video");
	_options.transform = settings.value("Transform").toString();
	_options.aspectRatio = settings.value("AspectRatio").toString();
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

	settings.beginGroup("ScreenCapture");
	_options.screenWidth = settings.value("Width", -1).toInt();
	_options.screenHeight = settings.value("Height", -1).toInt();
	_options.screenTop = settings.value("Top", -1).toInt();
	_options.screenLeft = settings.value("Left", -1).toInt();
	_options.screenFull = settings.value("Full", true).toBool();
	_options.screenFPS = settings.value("FPS", 30).toInt();
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
	settings.setValue("Transform", _options.transform);
	settings.setValue("AspectRatio", _options.aspectRatio);
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

	settings.beginGroup("ScreenCapture");
	settings.setValue("Width", _options.screenWidth);
	settings.setValue("Height", _options.screenHeight);
	settings.setValue("Top", _options.screenTop);
	settings.setValue("Left", _options.screenLeft);
	settings.setValue("Full", _options.screenFull);
	settings.setValue("FPS", _options.screenFPS);
	settings.endGroup();
}
