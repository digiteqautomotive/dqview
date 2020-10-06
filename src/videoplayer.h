#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include <vlc/vlc.h>

class QImage;
class Video;

class VideoPlayer : public QWidget
{
	Q_OBJECT

public:
	VideoPlayer(QWidget *parent = 0);
	~VideoPlayer();

	void setVideo(Video *video);
	void setImageDir(const QString &path) {_imageDir = path;}
	void setVideoDir(const QString &path) {_videoDir = path;}
	void setCodec(const QString &codec) {_codec = codec;}
	void setBitrate(unsigned bitrate) {_bitrate = bitrate;}

	QSize resolution() const;
	QString recordFile() const;

signals:
	void error(const QString &error);
	void stateChanged(bool playing);

public slots:
	void startStreaming();
	void startStreamingAndRecording();
	void stopStreaming();
	void captureImage();

private:
	void emitLoadStatus();
	static void handleEvent(const libvlc_event_t *event, void *userData);

	libvlc_instance_t *_vlc;
	libvlc_media_player_t *_mediaPlayer;
	libvlc_media_t *_media;

	QString _imageDir, _videoDir;
	QString _recordFile;
	QString _codec;
	unsigned _bitrate;
};

#endif // VIDEOPLAYER_H
