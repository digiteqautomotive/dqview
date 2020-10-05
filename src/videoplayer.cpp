#include <QVBoxLayout>
#include <QFileInfo>
#include <QDate>
#include <QTimer>
#include "sleep.h"
#include "video.h"
#include "videoplayer.h"


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
//	"-vvv"
};


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
		case libvlc_MediaPlayerVout:
			/* The resolution is ready AFTER the callback, so add some
			   additional time. The callback runs in a VLC thread context,
			   so we can not use a QTimer. */
			msleep(100);
			player->videoOutputReady();
			break;

		case libvlc_MediaPlayerEncounteredError:
			player->error("Error");
			break;
	}
}

VideoPlayer::VideoPlayer(QWidget *parent) : QWidget(parent),
  _vlc(libvlc_new(ARRAY_SIZE(vlcArguments), vlcArguments)),
  _mediaPlayer(0), _media(0)
{
	setAutoFillBackground(true);

	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, Qt::black);
	setPalette(palette);
}

VideoPlayer::~VideoPlayer()
{
	libvlc_release(_vlc);
}

void VideoPlayer::setVideo(Video *video)
{
	if (_media)
		libvlc_media_release(_media);
	_media = libvlc_media_new_location(_vlc, video->url().toLatin1().constData());

	libvlc_media_parse_with_options(_media, libvlc_media_parse_network, 5000);
}

void VideoPlayer::startStreaming()
{
	Q_ASSERT(!_mediaPlayer);

	_mediaPlayer = libvlc_media_player_new_from_media(_media);

	libvlc_event_manager_t* eventManager = libvlc_media_player_event_manager(
	  _mediaPlayer);
	libvlc_event_attach(eventManager, libvlc_MediaPlayerPlaying, handleEvent,
	  this);
	libvlc_event_attach(eventManager, libvlc_MediaPlayerStopped, handleEvent,
	  this);
	libvlc_event_attach(eventManager, libvlc_MediaPlayerVout, handleEvent,
	  this);

	libvlc_event_attach(eventManager, libvlc_MediaPlayerEncounteredError,
	  handleEvent, this);

#if defined(Q_OS_LINUX)
	libvlc_media_player_set_xwindow(_mediaPlayer, winId());
#elif defined(Q_OS_WIN)
	libvlc_media_player_set_hwnd(_mediaPlayer, winId());
#else
#error "unsupported platform"
#endif

	libvlc_media_player_play(_mediaPlayer);
}

void VideoPlayer::startStreamingAndRecording()
{
	QString time(QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss"));
	_recordFile = QString(_videoDir + "/" + time + ".mpeg");

	const char *recordingOptionPattern = "sout=#duplicate{dst=display,"
	  "dst='transcode{vcodec=h264,vb=1800}:standard{access=file,mux=ts,dst=%1}'}";
	QString recordingOption = QString(recordingOptionPattern).arg(_recordFile);
	libvlc_media_add_option(_media, recordingOption.toUtf8().constData());
	libvlc_media_add_option(_media, "v4l2-caching=100");

	startStreaming();

	emit recordingStateChanged(true);
}

void VideoPlayer::stopStreaming()
{
	if (!_mediaPlayer)
		return;

	libvlc_media_player_stop(_mediaPlayer);
	libvlc_media_player_release(_mediaPlayer);
	_mediaPlayer = 0;

	emit recordingStateChanged(false);
}

void VideoPlayer::captureImage()
{
	Q_ASSERT(_mediaPlayer);

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
