#include <QtGlobal>
#include "application.h"

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	qputenv("VLC_PLUGIN_PATH", "plugins");
#endif

	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

	Application app(argc, argv);
	return app.run();
}
