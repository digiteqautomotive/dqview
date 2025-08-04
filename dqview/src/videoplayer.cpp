#include <QVBoxLayout>
#include <QFileInfo>
#include <QDate>
#include <QTimer>
#include <QPainter>
#include <QStringList>
#include "video.h"
#include "videooutput.h"
#include "videoplayer.h"


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
			player->emitStateChanged(true);
			break;
		case libvlc_MediaPlayerStopped:
			player->emitStateChanged(false);
			break;
		case libvlc_MediaPlayerEncounteredError:
			player->error("Stream error. See the log file for more details.");
			break;
	}
}

void VideoPlayer::emitStateChanged(bool playing)
{
	if (!playing && _outputActive) {
		_display.stop();
		_display.close();
		_outputActive = false;
	}

	if (!playing) {
		QPainter p(this);
		p.fillRect(rect(), Qt::black);
		update();
	}

	emit stateChanged(playing);
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
	  << "--no-video-title-show"
	  << "--image-duration=-1";
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
  : QWidget(parent), _video(0), _vlc(0), _mediaPlayer(0), _display(0),
  _outputActive(false)
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

void VideoPlayer::startStreamingOut()
{
	unsigned den = 0, num = 0;
	libvlc_media_track_t **tracks;
	libvlc_media_t *media = createMedia();
	libvlc_media_parse(media);

	unsigned tn = libvlc_media_tracks_get(media, &tracks);
	for (unsigned i = 0; i < tn; i++) {
		const libvlc_media_track_t *t = tracks[i];
		if (t->i_type == libvlc_track_video) {
			// VLC uses framerate unlike v4l2 that uses period!
			num = t->video->i_frame_rate_den;
			den = t->video->i_frame_rate_num;
			break;
		}
	}
	libvlc_media_tracks_release(tracks, tn);

	if (!_display.open(num, den)) {
		emit error(tr("Error opening output device: ")
		  + _display.errorString());
		libvlc_media_release(media);
		return;
	}
	QSize size(_display.size());
	if (!size.isValid()) {
		_display.close();
		emit error(tr("Error fetching output device resolution: ")
		  + _display.errorString());
		libvlc_media_release(media);
		return;
	}
	PixelFormat format(_display.format());
	if (format == UnknownFormat) {
		_display.close();
		emit error(tr("Error fetching output pixel format: ")
		  + _display.errorString());
		libvlc_media_release(media);
		return;
	}

	QString codec = (format == RGB) ? "RV32" : "YUYV";
	QString rec = _video->show()
	  ? QString("sout=#duplicate{dst=display,dst='"
	  "transcode{vcodec=%1,acodec=null,width=%2,height=%3}:smem{"
	  "video-prerender-callback=%4,video-postrender-callback=%5,video-data=%6}'}")
	    .arg(codec, QString::number(size.width()), QString::number(size.height()),
	    QString::number((long long int)(intptr_t)(void*)VideoOutput::prerender()),
	    QString::number((long long int)(intptr_t)(void*)VideoOutput::postrender()),
	    QString::number((long long int)(intptr_t)(void*)&_display))
	  : QString("sout=#transcode{vcodec=%1,acodec=null,width=%2,height=%3}:smem{"
	  "video-prerender-callback=%4,video-postrender-callback=%5,video-data=%6}'}")
	    .arg(codec, QString::number(size.width()), QString::number(size.height()),
	    QString::number((long long int)(intptr_t)(void*)VideoOutput::prerender()),
	    QString::number((long long int)(intptr_t)(void*)VideoOutput::postrender()),
	    QString::number((long long int)(intptr_t)(void*)&_display));

	libvlc_media_add_option(media, rec.toUtf8().constData());
	libvlc_video_set_aspect_ratio(_mediaPlayer, _aspectRatio.isEmpty()
	  ? "Default" : _aspectRatio.constData());
	libvlc_media_player_set_media(_mediaPlayer, media);
	libvlc_media_release(media);

	libvlc_media_player_play(_mediaPlayer);

	_outputActive = _display.start();
	if (!_outputActive) {
		_display.close();
		emit error(tr("Error starting output device: ")
		  + _display.errorString());
		return;
	}
}

void VideoPlayer::startStreaming(bool record)
{
	if (record)
		startStreamingAndRecording();
	else
		startStreaming();
}

void VideoPlayer::startStreaming()
{
	libvlc_media_t *media = createMedia();

	libvlc_video_set_aspect_ratio(_mediaPlayer, _aspectRatio.isEmpty()
	  ? "Default" : _aspectRatio.constData());
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

	libvlc_video_set_aspect_ratio(_mediaPlayer, _aspectRatio.isEmpty()
	  ? "Default" : _aspectRatio.constData());
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
