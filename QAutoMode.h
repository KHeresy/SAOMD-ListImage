#pragma once

#include "QAdbController.h"
#include "ImageList.h"

#include <QWidget>
namespace Ui { class QAutoMode; };

class QAutoMode : public QWidget
{
	Q_OBJECT

public:
	QAutoMode(QWidget *parent = Q_NULLPTR);
	~QAutoMode();

protected slots:
	void runTest();
	void startCapture();
	void tabChanged(int);

signals:
	void newImage(CImageList*);
	void finished();

private:
	Ui::QAutoMode *ui;

	QAdbController mAdb;
};
