#include "ContourDraw.h"
#include <QtWidgets/QApplication>

#include "ContourClose.h"

// ���Կ���̨
//#pragma comment(linker, "/subsystem:console /entry:WinMainCRTStartup")

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ContouDraw w;
	w.show();
	return a.exec();
}
