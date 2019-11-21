#include "ContourDraw.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ContouDraw w;
	w.show();
	return a.exec();
}
