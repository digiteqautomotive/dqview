#include <QMainWindow>
#include "options.h"
#include "video.h"

class QCloseEvent;
class QKeyEvent;
class QLabel;
class QAction;
class QActionGroup;
class QSignalMapper;
class QToolBar;
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
	void configureDevice();
	void play(bool enable);
	void openOptions();
	void about();
	void updateTimer(int time);
	void streamError(const QString &error);
	void stateChanged(bool playing);
	void videoLoaded();
	void showLog();

private:
	void readSettings();
	void writeSettings();

	void createMenus();
	void createActions();
	QAction *streamAction(Stream *stream);
	QList<QAction*> deviceActions();
	QList<QAction*> streamActions();
	bool findStream(const StreamInfo &stream) const;
	void createToolbars();
	void createStatusBar();

	void stopStreaming();
	void startStreaming();
	void startRecording();
	void stopRecording();

	void showFullScreen(bool show);
	void showToolbars(bool show);

	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	QSize sizeHint() const {return QSize(640, 480);}

	VideoPlayer *_player;
	Device _device;

	QActionGroup *_deviceActionGroup;
	QActionGroup *_resizeActionGroup;
	QAction *_deviceSeparator;
	QAction *_openStreamAction;
	QAction *_configureDeviceAction;
	QAction *_playAction;
	QAction *_recordAction;
	QAction *_screenshotAction;
	QAction *_resizeVideoAction;
	QAction *_resizeWindowAction;
	QAction *_fullScreenAction;
	QAction *_optionsAction;
	QAction *_aboutAction;
	QAction *_exitAction;
	QAction *_showLogAction;

	QToolBar *_videoToolBar;
	QMenu *_deviceMenu;
	QSignalMapper *_streamSignalMapper;
	QList<QByteArray> _windowStates;

	QLabel *_deviceNameLabel;
	QLabel *_resolutionLabel;
	QLabel *_recordTimeLabel;
	QLabel *_recordFileLabel;

	Timer *_recordTimer;

	Options _options;
};
