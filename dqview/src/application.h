#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

class GUI;

class Application : QApplication
{
	Q_OBJECT

public:
	Application(int &argc, char **argv);
	~Application();

	int run();

private:
	GUI *_gui;
};

#endif // APPLICATION_H
