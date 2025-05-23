#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include <vlc/vlc.h>
#include "videooutput.h"
#include "log.h"

class QImage;
class Video;

class VideoPlayer : public QWidget
{
	Q_OBJECT

public:
	VideoPlayer(const QString &transform, QWidget *parent = 0);
	~VideoPlayer();

	void setVideo(Video *video) {_video = video;}
	void setImageDir(const QString &path) {_imageDir = path;}
	void setVideoDir(const QString &path) {_videoDir = path;}
	void setCodec(const QString &codec) {_codec = codec;}
	void setBitrate(unsigned bitrate) {_bitrate = bitrate;}
	void setAspectRatio(const QString &ratio) {_aspectRatio = ratio.toLatin1();}
	void setDisplay(Device *dev) {_display = VideoOutput(dev);}

	const Video *video() const {return _video;}
	QSize resolution() const;
	QString recordFile() const;

	Log &log() {return _log;}

signals:
	void error(const QString &error);
	void stateChanged(bool playing);

public slots:
	void startStreaming(bool record);
	void startStreamingOut();
	void stopStreaming();
	void captureImage();

private:
	void createArgs(const QString &transform);
	void createPlayer();
	libvlc_media_t *createMedia();
	void emitLoadStatus();
	static void handleEvent(const libvlc_event_t *event, void *userData);
	void emitStateChanged(bool playing);
	void startStreaming();
	void startStreamingAndRecording();

	Video *_video;

	const char **_argv;
	int _argc;
	libvlc_instance_t *_vlc;
	libvlc_media_player_t *_mediaPlayer;

	QString _imageDir, _videoDir;
	QString _recordFile;
	QString _codec;
	unsigned _bitrate;
	QByteArray _aspectRatio;

	VideoOutput _display;
	bool _outputActive;

	Log _log;
};

#endif // VIDEOPLAYER_H
