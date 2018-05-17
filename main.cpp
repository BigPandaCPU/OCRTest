#include "ocrtest.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	OCRTest w;
	w.show();
	return a.exec();
}
