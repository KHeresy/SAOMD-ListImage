#pragma once

#include <QWidget>
#include "ui_QUpdateCheck.h"

class QNetworkReply;

class QUpdateCheck : public QWidget
{
	Q_OBJECT

public:
	QUpdateCheck(QWidget *parent = Q_NULLPTR);

public slots:
	void slotCheckUpdate();

private slots:
	void requestFinished(QNetworkReply*);

public:
	Ui::QUpdateCheck ui;

private:
	QUrl qUpdateUrl;
};
