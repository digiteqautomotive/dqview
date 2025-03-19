#include <QtGlobal>
#include "application.h"
#include "streaminfo.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	qRegisterMetaTypeStreamOperators<StreamInfo>("Stream");
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#else
	qRegisterMetaType<StreamInfo>("StreamInfo");
#endif

	Application app(argc, argv);
	return app.run();
}
