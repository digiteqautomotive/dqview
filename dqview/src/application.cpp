#include <QDir>
#include "application.h"
#include "gui.h"

Application::Application(int &argc, char **argv) : QApplication(argc, argv)
{
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	QString pluginsDir (QDir(applicationDirPath()).absoluteFilePath("plugins"));
	qputenv("VLC_PLUGIN_PATH", pluginsDir.toUtf8());
#endif
	_gui = new GUI();
}

Application::~Application()
{
	delete _gui;
}

int Application::run()
{
	_gui->show();
	return exec();
}
