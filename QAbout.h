#pragma once

#include <QDialog>
#include "ui_QAbout.h"

class QAbout : public QDialog
{
	Q_OBJECT

public:
	QAbout(QWidget *parent = Q_NULLPTR);

private:
	Ui::QAbout ui;
};
