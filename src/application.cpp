#include "application.h"
#include "gui.h"

Application::Application(int &argc, char **argv) : QApplication(argc, argv)
{
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
