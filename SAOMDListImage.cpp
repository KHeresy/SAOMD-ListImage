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
#include <QDesktopServices>
#include <QActionGroup>
#pragma endregion

SAOMDListImage::SAOMDListImage(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	ui.graphicsView->setScene(new QGraphicsScene());
	ui.graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
	//ui.graphicsView->setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));
	setAcceptDrops(true);
	connect(ui.graphicsView->scene(), &QGraphicsScene::selectionChanged, this, &SAOMDListImage::slotSelectChanged);

	setWindowTitle(windowTitle() + SQOMDLI_VER);

	m_qUpdateCheck = new QUpdateCheck();
	ui.dockUpdateer->setWidget(m_qUpdateCheck);

	m_qAbout = new QAbout(this);
	m_qAbout->setModal(true);

	#pragma region Group zoom action
	QActionGroup* pGroup = new QActionGroup(this);
	pGroup->addAction(ui.actionFit_Image);
	pGroup->addAction(ui.actionFit_Height);
	pGroup->addAction(ui.actionFit_Width);
	pGroup->addAction(ui.actionZoomFull);
	pGroup->addAction(ui.actionZoomHalf);
	pGroup->addAction(ui.actionZoomCustom);
	#pragma endregion

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

	// zoom
	CScaleControlView::EScaleMode eZoomMode = (CScaleControlView::EScaleMode)qSettings.value("zoom/mode", (int)(CScaleControlView::EScaleMode::FitImage)).toInt();
	float fScale = qSettings.value("zoom/scale", 1.0f).toFloat();
	ui.graphicsView->setScale(fScale);
	ui.graphicsView->setScaleMode(eZoomMode);
	#pragma endregion
}

SAOMDListImage::~SAOMDListImage()
{
}

QSettings SAOMDListImage::getSettings()
{
	return QSettings("setting.ini", QSettings::IniFormat);
}

void SAOMDListImage::closeEvent(QCloseEvent* pEvent)
{
	#pragma region Save setting
	QSettings qSettings = getSettings();

	// Version
	qSettings.setValue("AppVer", SQOMDLI_VER);

	// Windows
	qSettings.setValue("ui/geometry", saveGeometry());
	qSettings.setValue("ui/windowState", saveState());

	// Layout
	qSettings.setValue("layout/ColumnNum", ui.hsColumnNum->value());
	qSettings.setValue("layout/border", ui.hsItemBorder->value());

	// Update Checker
	qSettings.setValue("updater/auto", m_qUpdateCheck->ui.cbAutoCheck->isChecked());

	// zoom
	qSettings.setValue("zoom/mode", (int)ui.graphicsView->getMode());
	qSettings.setValue("zoom/scale", ui.graphicsView->getScale());
	#pragma endregion

	slotClear();

	QMainWindow::closeEvent(pEvent);
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

	auto pScene = ui.graphicsView->scene();
	for (auto& rIL : vResult)
	{
		CImageList* mIL = rIL.result();
		m_vImageList.push_back(mIL);
		if (mIL->isVaild())
		{
			if (mIL->size() > 0)
			{
				for (const auto& qItem : mIL->m_vItems)
				{
					auto pSceneItem = pScene->addPixmap(QPixmap::fromImage(qItem));
					pSceneItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
					//pSceneItem->setFlag(QGraphicsItem::ItemIsMovable, true);

					m_vItems.push_back(pSceneItem);
				}
			}
		}
		else
		{

		}
	}

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

void SAOMDListImage::slotZoom(int iVal)
{
	ui.graphicsView->setScaleMode(CScaleControlView::EScaleMode::Custom);
	ui.graphicsView->setScale(0.01f * iVal);
}

void SAOMDListImage::slotModeChanged(CScaleControlView::EScaleMode eMode)
{
	using EMode = CScaleControlView::EScaleMode;

	switch (eMode)
	{
	case EMode::FitImage:
		ui.actionFit_Image->setChecked(true);
		break;

	case EMode::FitWidth:
		ui.actionFit_Width->setChecked(true);
		break;

	case EMode::FitHeight:
		ui.actionFit_Height->setChecked(true);
		break;

	case EMode::Full:
		ui.actionZoomFull->setChecked(true);
		break;

	case EMode::Half:
		ui.actionZoomHalf->setChecked(true);
		break;

	case EMode::Custom:
		ui.actionZoomCustom->setChecked(true);
		break;
	}
}

void SAOMDListImage::slotSclaeChanged(float fScale)
{
	int iScale = 100 * fScale;
	if (ui.hsZoom->value() != iScale)
	{
		ui.hsZoom->blockSignals(true);
		ui.hsZoom->setValue(iScale);
		ui.hsZoom->blockSignals(false);
	}
}

void SAOMDListImage::slotAbout()
{
	m_qAbout->show();
}

void SAOMDListImage::slotDemoVideo()
{
	QDesktopServices::openUrl(QUrl(DEMO_VIDEO_URL));
}

void SAOMDListImage::slotHowToUse()
{
	QDesktopServices::openUrl(QUrl("https://github.com/KHeresy/SAOMD-ListImage/blob/master/README.md#%E4%BD%BF%E7%94%A8%E6%96%B9%E5%BC%8F-how-to-use"));
}

void SAOMDListImage::slotSelectChanged()
{
	if (ui.graphicsView->scene()->selectedItems().size() > 0)
		ui.pbDeleteSelected->setEnabled(true);
	else
		ui.pbDeleteSelected->setEnabled(false);
}

void SAOMDListImage::slotDeleteSelected()
{
	auto pScene = ui.graphicsView->scene();
	QList<QGraphicsItem*> vSItems = pScene->selectedItems();

	for (auto pItem : vSItems)
	{
		for (auto pCItem : m_vItems)
		{
			if (pCItem == pItem)
			{
				pScene->removeItem(pItem);
				m_vItems.removeOne(pCItem);
			}
		}
	}

	updateLayout();
}

void SAOMDListImage::updateLayout()
{
	ui.leNumber->setText(QString("%1").arg(m_vItems.size()));

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
	ui.leImageSize->setText(QString("%1 x %2").arg(rect.width()).arg(rect.height()));

	ui.graphicsView->UpdateViewScale();
}
#pragma endregion
