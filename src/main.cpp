#include <QtGlobal>
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
#include <Windows.h>
#endif
#include "application.h"

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
	if (AttachConsole(ATTACH_PARENT_PROCESS)) {
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
	}

	qputenv("VLC_PLUGIN_PATH", "plugins");
#endif

	Application app(argc, argv);
	return app.run();
}
