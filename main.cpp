#include "SAOMDListImage.h"
#include <QtWidgets/QApplication>
#include <QTranslator>

#include "QAdbController.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QTranslator qtTranslator;
	if(qtTranslator.load(QLocale(), QLatin1String("saomd-listimage"), QLatin1String("_")))
		app.installTranslator(&qtTranslator);

	SAOMDListImage w;
	w.show();
	return app.exec();
}
