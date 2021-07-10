#include "SAOMDListImage.h"
#include "Define.h"

#pragma region Qt Headers
#include <QDialog>
#include <QDropEvent>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QImage>
#include <QMessageBox>
#include <QMimeData>
#include <QFuture>
#include <QtConcurrent>
#pragma endregion

SAOMDListImage::SAOMDListImage(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	ui.graphicsView->setScene(new QGraphicsScene());
	//ui.graphicsView->setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));
	setAcceptDrops(true);

	setWindowTitle(windowTitle() + SQOMDLI_VER);

	m_qUpdateCheck = new QUpdateCheck();
	ui.dockUpdateer->setWidget(m_qUpdateCheck);

	m_qAbout = new QAbout(this);
	m_qAbout->setModal(true);

	#pragma region Read setting
	QSettings qSettings = getSettings();
	// Windows
	restoreGeometry(qSettings.value("ui/geometry").toByteArray());
	restoreState(qSettings.value("ui/windowState").toByteArray());

	// Layout
	ui.hsColumnNum->setValue(qSettings.value("layout/ColumnNum", 16).toInt());
	ui.hsItemBorder->setValue(qSettings.value("layout/border", 1).toInt());

	// Update Checker
	bool bUpdate = qSettings.value("updater/auto", false).toBool();
	m_qUpdateCheck->ui.cbAutoCheck->setChecked(bUpdate);
	if (bUpdate)
		m_qUpdateCheck->slotCheckUpdate();
	#pragma endregion
}

SAOMDListImage::~SAOMDListImage()
{
	#pragma region Save setting
	QSettings qSettings = getSettings();
	// Windows
	qSettings.setValue("ui/geometry", saveGeometry());
	qSettings.setValue("ui/windowState", saveState());

	// Layout
	qSettings.setValue("layout/ColumnNum", ui.hsColumnNum->value());
	qSettings.setValue("layout/border", ui.hsItemBorder->value());

	// Update Checker
	qSettings.setValue("updater/auto", m_qUpdateCheck->ui.cbAutoCheck->isChecked());
	#pragma endregion

	slotClear();
}

QSettings SAOMDListImage::getSettings()
{
	return QSettings("setting.ini", QSettings::IniFormat);
}

void SAOMDListImage::dragEnterEvent(QDragEnterEvent* pEvent)
{
	if (pEvent->mimeData()->hasUrls())
		pEvent->acceptProposedAction();
}

void SAOMDListImage::dropEvent(QDropEvent* pEvent)
{
	const QMimeData* pMimeData = pEvent->mimeData();
	if (pMimeData->hasUrls())
	{
		QStringList pathList;
		for (const auto& qUrl : pMimeData->urls())
			pathList.append(qUrl.toLocalFile());

		loadFiles(pathList);
	}
}

void SAOMDListImage::loadFiles(const QStringList & aFileList)
{
	QList<QFuture<CImageList*>> vResult;
	for (const QString& sFile : aFileList)
		vResult.push_back(QtConcurrent::run([sFile]() {
			return new CImageList(sFile);
		}));

	for (auto& rIL : vResult)
	{
		CImageList* mIL = rIL.result();
		m_vImageList.push_back(mIL);
		if (mIL->isVaild())
		{
			if (mIL->size() > 0)
			{
				for (const auto& qItem : mIL->m_vItems)
					m_vItems.push_back(ui.graphicsView->scene()->addPixmap(QPixmap::fromImage(qItem)));
			}
		}
		else
		{

		}
	}

	ui.leNumber->setText(QString("%1").arg(m_vItems.size()));
	updateLayout();
}

#pragma region slot functions
void SAOMDListImage::slotLoad()
{
	QStringList aFiles = QFileDialog::getOpenFileNames( this, tr("Select one or more files"), "", "Images (*.png *.jpg);;Any files (*.*)");
	if (aFiles.size() > 0)
		loadFiles(aFiles);
}

void SAOMDListImage::slotClear()
{
	auto pScene = ui.graphicsView->scene();
	for (auto pItem : m_vItems)
	{
		pScene->removeItem(pItem);
		delete pItem;
	}
	m_vItems.clear();

	for (auto pIL : m_vImageList)
		delete pIL;
	m_vImageList.clear();
}

void SAOMDListImage::slotSave()
{
	QString sFile = QFileDialog::getSaveFileName(this, tr("Input the file to save"), "", "Images (*.png)");
	if (!sFile.isEmpty())
	{
		QImage image(ui.graphicsView->scene()->sceneRect().size().toSize(), QImage::Format_ARGB32);
		image.fill(Qt::transparent);

		QPainter painter(&image);
		ui.graphicsView->scene()->render(&painter);
		image.save(sFile);
	}
}

void SAOMDListImage::slotAbout()
{
	m_qAbout->show();
}
#pragma endregion

void SAOMDListImage::updateLayout()
{
	int iCol = ui.hsColumnNum->value();
	int iBorder = ui.hsItemBorder->value();
	auto pScene = ui.graphicsView->scene();

	int x = 0, y = 0, num = -1, row = 0;
	for (auto& pItem : m_vItems)
	{
		if (++num >= iCol)
		{
			++row;
			num = 0;
			x = 0;
			y += (pItem->boundingRect().height() + iBorder);
		}

		pItem->setPos(x, y);
		x += (iBorder + pItem->boundingRect().width() );
	}

	ui.leArraySize->setText(QString("%1 x %2").arg(iCol).arg(row + 1));

	pScene->setSceneRect(pScene->itemsBoundingRect());
	auto rect = pScene->sceneRect();
	ui.graphicsView->fitInView(rect,Qt::KeepAspectRatio);
	ui.leImageSize->setText(QString("%1 x %2").arg(rect.width()).arg(rect.height()));
}
