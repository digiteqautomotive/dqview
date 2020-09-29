#include <QMainWindow>
#include "options.h"

class QCloseEvent;
class QLabel;
class QAction;
class QActionGroup;
class QSignalMapper;
class VideoPlayer;
class Stream;
class Timer;

class GUI : public QMainWindow
{
	Q_OBJECT

public:
	GUI();

private slots:
	void openStream();
	void openDevice(QObject *device);
	void play(bool enable);
	void openOptions();
	void about();
	void updateTimer(int time);
	void streamError(const QString &error);
	void stateChange(bool playing);
	void recordingStateChange(bool recording);

private:
	void readSettings();
	void writeSettings();

	void createMenus();
	void createActions();
	QAction *streamAction(Stream *stream);
	QList<QAction*> cameraActions();
	QList<QAction*> streamActions();
	bool findStream(const StreamInfo &stream) const;
	void createToolbars();
	void createStatusBar();

	void stopStreaming();
	void startStreaming();
	void startRecording();
	void stopRecording();
	void startStreamingAndRecording();

	void closeEvent(QCloseEvent *event);
	QSize sizeHint() const {return QSize(640, 480);}

	VideoPlayer *_player;

	QActionGroup *_deviceActionGroup;
	QAction *_deviceSeparator;
	QAction *_openStreamAction;
	QAction *_playAction;
	QAction *_recordAction;
	QAction *_screenshotAction;
	QAction *_resizeAction;
	QAction *_optionsAction;
	QAction *_aboutAction;
	QAction *_exitAction;

	QMenu *_deviceMenu;
	QSignalMapper *_streamSignalMapper;

	QLabel *_deviceNameLabel;
	QLabel *_resolutionLabel;
	QLabel *_recordTimeLabel;
	QLabel *_recordFileLabel;

	Timer *_recordTimer;

	Options _options;
};
