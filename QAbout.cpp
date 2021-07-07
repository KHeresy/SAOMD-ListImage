#include "QAbout.h"
#include "Define.h"

#include <QDesktopServices>
#include <QUrl>

QAbout::QAbout(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.label->setOpenExternalLinks(true);
	ui.labelTitle->setText(ui.labelTitle->text() + SQOMDLI_VER);
}
