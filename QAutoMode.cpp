#include "QAutoMode.h"

#include "ui_QAutoMode.h"

#include <QDir>

QAutoMode::QAutoMode(QWidget *parent)
	: QWidget(parent)
{
	ui = new Ui::QAutoMode();
	ui->setupUi(this);
}

QAutoMode::~QAutoMode()
{
	delete ui;
}

void QAutoMode::runTest()
{
	QString sTestResult;
	
	QStringList vDevice = mAdb.listDevices();
	if (vDevice.size() > 0)
	{
		sTestResult = tr("Found first device: ") + vDevice[0] + "\n";
		
		QImage vTest = mAdb.getScreen();
		sTestResult += (tr("Screen size: ") + QString("%1 / %2").arg(vTest.width()).arg(vTest.height()));
	}
	else
	{
		sTestResult = tr("No devices found");
	}

	ui->labTest->setText(sTestResult);
}

void QAutoMode::startCapture()
{
	int iTotalNum = ui->sbTotalNum->value();
	QString sPath = ui->leImagePath->text();

	QDir qDir;
	if (!qDir.exists(sPath))
		qDir.mkpath(sPath);

	int iIdx = 0;
	QString sFileName = QString(sPath + "\\%1.png").arg(iIdx);
	mAdb.saveScreen(sFileName);
	CImageList* pImageList = new CImageList(sFileName);

	auto c = pImageList->size();
	emit newImage(pImageList);

	emit finished();
}

void QAutoMode::tabChanged(int iIdx)
{
	if (iIdx == 0)
		ui->pbStart->setEnabled(false);
	else
		ui->pbStart->setEnabled(true);
}
