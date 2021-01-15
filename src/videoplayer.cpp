#include <QVBoxLayout>
#include <QFileInfo>
#include <QDate>
#include <QTimer>
#include <QPainter>
#include "video.h"
#include "videoplayer.h"


#define MAX_LOG_SIZE 1000

#define ARRAY_SIZE(array) \
  (sizeof(array) / sizeof(array[0]))

static const char *vlcArguments[] = {
	"--intf=dummy",
	"--ignore-config",
	"--no-media-library",
	"--no-one-instance",
	"--no-osd",
	"--no-snapshot-preview",
	"--no-stats",
	"--no-video-title-show",
	//"-vvv"
};

static const char *vlcArgumentsFlip[] = {
	"--intf=dummy",
	"--ignore-config",
	"--no-media-library",
	"--no-one-instance",
	"--no-osd",
	"--no-snapshot-preview",
	"--no-stats",
	"--no-video-title-show",
	"--video-filter=transform",
	"--transform-type=vflip",
	//"-vvv"
};

static void logCb(void *data, int level, const libvlc_log_t *ctx,
  const char *fmt, va_list args)
{
	Q_UNUSED(ctx);

	Log *log = static_cast<Log *>(data);
	QString msg(QString::vasprintf(fmt, args));

	if (log->size() == MAX_LOG_SIZE)
		log->removeFirst();
	log->append(LogEntry(QTime::currentTime(), level, msg));
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

VideoPlayer::VideoPlayer(QWidget *parent)
  : QWidget(parent), _video(0), _vlc(0), _mediaPlayer(0)
{
	setAutoFillBackground(true);
	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, Qt::black);
	setPalette(palette);

	_vlc = libvlc_new(ARRAY_SIZE(vlcArguments), vlcArguments);
	createPlayer();

	_bitrate = 1800;
	_codec = QString("h264");
	_flip = false;
}

VideoPlayer::~VideoPlayer()
{
	libvlc_media_player_release(_mediaPlayer);
	libvlc_release(_vlc);
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

void VideoPlayer::setFlip(bool flip)
{
	if (_flip == flip)
		return;
	_flip = flip;

	/*
		There is no other way in libvlc 3.x to set the video output filters
		than to do it through the libvlc_new() parameters. However, libvlc_new()
		called multiple times in a single process is very fragile and leads to
		crashes/deadlocks on some configurations/systems...
	*/
	libvlc_media_player_release(_mediaPlayer);
	libvlc_release(_vlc);

	_vlc = flip
	  ? libvlc_new(ARRAY_SIZE(vlcArgumentsFlip), vlcArgumentsFlip)
	  : libvlc_new(ARRAY_SIZE(vlcArguments), vlcArguments);

	createPlayer();
}
