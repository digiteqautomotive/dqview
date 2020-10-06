#include <QVBoxLayout>
#include <QFileInfo>
#include <QDate>
#include <QTimer>
#include <QPainter>
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
		case libvlc_MediaPlayerEncounteredError:
			player->error("Stream error. See stdout for more details.");
			break;
	}
}

VideoPlayer::VideoPlayer(QWidget *parent) : QWidget(parent), _media(0)
{
	setAutoFillBackground(true);
	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, Qt::black);
	setPalette(palette);

	_vlc = libvlc_new(ARRAY_SIZE(vlcArguments), vlcArguments);
	_mediaPlayer = libvlc_media_player_new(_vlc);

	libvlc_event_manager_t* eventManager = libvlc_media_player_event_manager(
	  _mediaPlayer);
	libvlc_event_attach(eventManager, libvlc_MediaPlayerPlaying, handleEvent,
	  this);
	libvlc_event_attach(eventManager, libvlc_MediaPlayerStopped, handleEvent,
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
}

VideoPlayer::~VideoPlayer()
{
	if (_media)
		libvlc_media_release(_media);
	libvlc_media_player_release(_mediaPlayer);
	libvlc_release(_vlc);
}

void VideoPlayer::setVideo(Video *video)
{
	_media = libvlc_media_new_location(_vlc, video->url().toLatin1().constData());
	libvlc_media_player_set_media(_mediaPlayer, _media);
}

void VideoPlayer::startStreaming()
{
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
