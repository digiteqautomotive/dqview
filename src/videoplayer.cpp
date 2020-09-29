#include <QVBoxLayout>
#include <QFileInfo>
#include <QDate>
#include "video.h"
#include "videoplayer.h"


VideoPlayer::VideoPlayer(QWidget *parent) : QWidget(parent)
{
	_video = 0;
	_player = new QtAV::AVPlayer(this);
	_vo = new QtAV::VideoOutput(this);
	_player->setRenderer(_vo);
	_player->setBufferMode(QtAV::BufferTime);
	_player->setBufferValue(1000);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	layout->addWidget(_vo->widget());
	setLayout(layout);

	QVariantHash opt;
	opt["protocol_whitelist"] = "file,udp,tcp,rtp";
	_player->setOptionsForFormat(opt);

	_avt = new QtAV::AVTranscoder(this);
	_avt->setMediaSource(_player);
	//_avt->setOutputOptions(muxopt);
	//_avt->setOutputFormat(fmt);
	_avt->createVideoEncoder();
	QtAV::VideoEncoder *venc = _avt->videoEncoder();
	venc->setCodecName("libx264");
	venc->setBitRate(1024*1024);

	connect(_avt, &QtAV::AVTranscoder::started, this,
	  &VideoPlayer::emitRecordingStart);
	connect(_avt, &QtAV::AVTranscoder::stopped, this,
	  &VideoPlayer::emitRecordingStop);

	_player->videoCapture()->setAutoSave(false);
	connect(_player->videoCapture(), &QtAV::VideoCapture::imageCaptured, this,
	  &VideoPlayer::saveImage);

	connect(_player, &QtAV::AVPlayer::error, this, &VideoPlayer::emitError);
	connect(_player, &QtAV::AVPlayer::stateChanged, this,
	  &VideoPlayer::emitStateChange);
}

void VideoPlayer::setVideo(Video *video)
{
	_video = video;
}

void VideoPlayer::startStreaming()
{
	qDebug() << "URL" << _video->url();
	_player->play(_video->url());
}

void VideoPlayer::startStreamingAndRecording()
{
	QString time(QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss"));
	QString recordFile(_videoDir + "/" + time + ".mpeg");

	qDebug() << "RECORD" << recordFile;
	_avt->setOutputMedia(recordFile);
	_avt->start();

	qDebug() << "URL" << _video->url();
	_player->play(_video->url());
}

void VideoPlayer::stopStreaming()
{
	if (_player->isPlaying()) {
		qDebug() << "STOP";
		_player->stop();
	}
	if (_avt->isRunning()) {
		qDebug() << "RECORD STOP";
		_avt->stop();
	}
}

void VideoPlayer::captureImage()
{
	_player->videoCapture()->capture();
}

void VideoPlayer::saveImage(const QImage& image)
{
	QString time(QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss"));
	QString path(_imageDir + "/" + time + ".png");
	qDebug() << "IMAGE" << path;
	image.save(path);
}

void VideoPlayer::emitError(const QtAV::AVError &err)
{
	emit error(err.ffmpegErrorString());
}

void VideoPlayer::emitStateChange(QtAV::AVPlayer::State state)
{
	switch (state) {
		case QtAV::AVPlayer::PlayingState:
			emit stateChanged(true);
			break;
		default:
			emit stateChanged(false);
	}
}

void VideoPlayer::emitRecordingStart()
{
	emit recordingStateChanged(true);
}

void VideoPlayer::emitRecordingStop()
{
	emit recordingStateChanged(false);
}

QSize VideoPlayer::resolution() const
{
	return QSize(_player->statistics().video_only.width,
	  _player->statistics().video_only.height);
}

QString VideoPlayer::recordFile() const
{
	return _avt->outputFile();
}
