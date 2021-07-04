#include "QAbout.h"

#include <QDesktopServices>
#include <QUrl>

QAbout::QAbout(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.label->setOpenExternalLinks(true);
	//ui.label->setText("<a href=https://kheresy.wordpress.com/>https://kheresy.wordpress.com/</a>");
}

QAbout::~QAbout()
{
}
