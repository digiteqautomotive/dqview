#include <QtAVWidgets>
#include "application.h"

int main(int argc, char *argv[])
{
	QtAV::Widgets::registerRenderers();
	Application app(argc, argv);
	return app.run();
}
