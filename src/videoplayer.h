#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QtAV>
#include <QtAV/AVTranscoder.h>
#include <QWidget>

class QImage;
class Video;

class VideoPlayer : public QWidget
{
	Q_OBJECT

public:
	VideoPlayer(QWidget *parent = 0);

	const Video *video() const {return _video;}
	void setVideo(Video *video);

	void setImageDir(const QString &path) {_imageDir = path;}
	void setVideoDir(const QString &path) {_videoDir = path;}

	QSize resolution() const;
	QString recordFile() const;

signals:
	void error(const QString &error);
	void stateChanged(bool playing);
	void recordingStateChanged(bool recording);

public slots:
	void startStreaming();
	void startStreamingAndRecording();
	void stopStreaming();
	void captureImage();

private slots:
	void saveImage(const QImage &image);
	void emitError(const QtAV::AVError &err);
	void emitStateChange(QtAV::AVPlayer::State state);
	void emitRecordingStart();
	void emitRecordingStop();

private:
	Video *_video;
	QtAV::VideoOutput *_vo;
	QtAV::AVPlayer *_player;
	QtAV::AVTranscoder *_avt;

	QString _imageDir, _videoDir;
};

#endif // VIDEOPLAYER_H
