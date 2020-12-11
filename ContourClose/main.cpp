#include "ContourDraw.h"
#include <QtWidgets/QApplication>

#include "ContourClose.h"

// µ÷ÊÔ¿ØÖÆÌ¨
//#pragma comment(linker, "/subsystem:console /entry:WinMainCRTStartup")

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ContouDraw w;
	w.show();
	return a.exec();
}
