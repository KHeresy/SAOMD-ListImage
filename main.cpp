#include "SAOMDListImage.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SAOMDListImage w;
	w.show();
	return a.exec();
}
