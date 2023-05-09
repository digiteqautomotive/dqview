#include <QVBoxLayout>
#include <QFileInfo>
#include <QDate>
#include <QTimer>
#include <QPainter>
#include <QStringList>
#include "video.h"
#include "videoplayer.h"
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
#include "fg4.h"
#endif


#define MAX_LOG_SIZE 1000

static void logCb(void *data, int level, const libvlc_log_t *ctx,
  const char *fmt, va_list args)
{
	Q_UNUSED(ctx);

	Log *log = static_cast<Log *>(data);
	QString msg(QString::vasprintf(fmt, args));

	log->mutex.lock();
	if (log->list.size() == MAX_LOG_SIZE)
		log->list.removeFirst();
	log->list.append(LogEntry(QTime::currentTime(), level, msg));
	log->mutex.unlock();
}


void VideoPlayer::handleEvent(const libvlc_event_t *event, void *userData)
{
	VideoPlayer *player = static_cast<VideoPlayer*>(userData);

	switch (event->type) {
		case libvlc_MediaPlayerPlaying:
			player->stateChanged(true);
			break;
		case libvlc_MediaPlayerStopped:
			player->stateChanged(false);
			break;
		case libvlc_MediaPlayerEncounteredError:
			player->error("Stream error. See the log file for more details.");
			break;
	}
}

void VideoPlayer::createArgs(const QString &transform)
{
	QStringList list;

	list << "--intf=dummy"
	  << "--ignore-config"
	  << "--no-media-library"
	  << "--no-one-instance"
	  << "--no-osd"
	  << "--no-snapshot-preview"
	  << "--no-stats"
	  << "--no-video-title-show";
	if (!transform.isEmpty()) {
		list << "--video-filter=transform";
		list << "--transform-type=" + transform;
	}

	_argc = list.size();
	_argv = new const char*[_argc];

	for (int i = 0; i < list.size(); i++) {
		QByteArray ba(list.at(i).toLatin1());
		_argv[i] = new char[ba.size() + 1];
		strcpy((char*)_argv[i], ba.data());
	}
}

VideoPlayer::VideoPlayer(const QString &transform, QWidget *parent)
  : QWidget(parent), _video(0), _vlc(0), _mediaPlayer(0)
{
	setAutoFillBackground(true);
	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, Qt::black);
	setPalette(palette);

	/* There is no other way in libvlc 3.x to set the video output filters
	   than to do it through the libvlc_new() parameters. At the same time,
	   changing the libvlc instance using another libvlc_new() crashes the
	   process...
	*/
	createArgs(transform);
	_vlc = libvlc_new(_argc, _argv);
	createPlayer();

	_bitrate = 1800;
	_codec = QString("h264");
}

VideoPlayer::~VideoPlayer()
{
	libvlc_media_player_release(_mediaPlayer);
	libvlc_release(_vlc);

	for (int i = 0; i < _argc; i++)
		delete[] _argv[i];
	delete[] _argv;
}

void VideoPlayer::createPlayer()
{
	_mediaPlayer = libvlc_media_player_new(_vlc);

	libvlc_event_manager_t* eventManager = libvlc_media_player_event_manager(
	  _mediaPlayer);
	libvlc_event_attach(eventManager, libvlc_MediaPlayerPlaying, handleEvent,
	  this);
	libvlc_event_attach(eventManager, libvlc_MediaPlayerStopped, handleEvent,
	  this);
	libvlc_event_attach(eventManager, libvlc_MediaPlayerEncounteredError,
	  handleEvent, this);

	libvlc_log_set(_vlc, logCb, &_log);

#if defined(Q_OS_LINUX)
	libvlc_media_player_set_xwindow(_mediaPlayer, winId());
#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	libvlc_media_player_set_hwnd(_mediaPlayer, reinterpret_cast<void*>(winId()));
#else
#error "unsupported platform"
#endif
}

libvlc_media_t *VideoPlayer::createMedia()
{
	Q_ASSERT(_video);

	QStringList list = _video->url().split(" :");
	libvlc_media_t *media = libvlc_media_new_location(_vlc,
	  list.first().toLatin1().constData());
	for (int i = 1; i < list.count(); i++)
		libvlc_media_add_option(media, list.at(i).toLatin1().constData());

	return media;
}

void VideoPlayer::startStreaming()
{
	libvlc_media_t *media = createMedia();

	adjustAspectRatio();
	libvlc_media_player_set_media(_mediaPlayer, media);
	libvlc_media_release(media);
	libvlc_media_player_play(_mediaPlayer);
}

void VideoPlayer::startStreamingAndRecording()
{
	libvlc_media_t *media = createMedia();

	QString time(QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss"));
	_recordFile = QString(_videoDir + "/" + time + ".mpeg");

	QString rec = QString("sout=#duplicate{dst=display,dst='transcode{vcodec=%2"
	  ",vb=%3}:standard{access=file,mux=ts,dst=%1}'}")
	  .arg(_recordFile, _codec, QString::number(_bitrate));
	libvlc_media_add_option(media, rec.toUtf8().constData());

	adjustAspectRatio();
	libvlc_media_player_set_media(_mediaPlayer, media);
	libvlc_media_release(media);
	libvlc_media_player_play(_mediaPlayer);
}

void VideoPlayer::stopStreaming()
{
	libvlc_media_player_stop(_mediaPlayer);

	QPainter p(this);
	p.fillRect(rect(), Qt::black);
	update();
}

#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
QPoint VideoPlayer::aspectRatio()
{
	Device dev(_video->device());
	long resolution;

	if (!dev.isValid())
		return QPoint();
	IFG4InputConfig *config = (IFG4InputConfig*)dev.config();
	if (!config || FAILED(config->GetDetectedResolution(&resolution)))
		return QPoint();

	return QPoint(resolution >> 16, resolution & 0xFFFF);
}
#endif

void VideoPlayer::adjustAspectRatio()
{
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	/* On Windows, always explicitly set the aspect ratio to the real signal
	   aspect ratio (when no user aspect ratio is defined). The aspect ratio is
	   _wrong_ on Windows when the camera is not 4:3... */
	QPoint ratio(aspectRatio());
	QByteArray ar = (ratio.isNull())
	  ? "Default"
	  : QByteArray::number(ratio.x()) + ":" + QByteArray::number(ratio.y());

	libvlc_video_set_aspect_ratio(_mediaPlayer, _aspectRatio.isEmpty()
	  ? ar.constData() : _aspectRatio.constData());
#else
	libvlc_video_set_aspect_ratio(_mediaPlayer,
	  _aspectRatio.isEmpty() ? "Default" : _aspectRatio.constData());
#endif
}

void VideoPlayer::captureImage()
{
	QString time(QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss"));
	QString path(_imageDir + "/" + time + ".png");

	libvlc_video_take_snapshot(_mediaPlayer, 0, path.toUtf8().constData(), 0, 0);
}

QSize VideoPlayer::resolution() const
{
	unsigned width, height;
	libvlc_video_get_size(_mediaPlayer, 0, &width, &height);
	return QSize(width, height);
}

QString VideoPlayer::recordFile() const
{
	return _recordFile;
}
