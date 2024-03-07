#include <QtGlobal>
#include "application.h"
#include "streaminfo.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	qRegisterMetaTypeStreamOperators<StreamInfo>("Stream");
#else
	qRegisterMetaType<StreamInfo>("StreamInfo");
#endif

	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

	Application app(argc, argv);
	return app.run();
}
