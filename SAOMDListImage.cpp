#include "SAOMDListImage.h"

#pragma region Qt Headers
#include <QDropEvent>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QImage>
#include <QMessageBox>
#include <QMimeData>
#pragma endregion

#pragma region Internal process functions
QRect detectRow(const QImage& qImage, const QColor& mBGColorBase, const QRect& qRange)
{
	const QRgb* pImageData = (const QRgb*)(qImage.constBits());

	int iRowBegin = -1, iRowEnd = -2;
	bool bInRange = false;
	for (int y = qRange.top(); y < qRange.bottom(); ++y)
	{
		bool bDiff = false;
		for (int x = qRange.left(); x < qRange.width() / 4; ++x)
		{
			QColor mColor = QColor::fromRgba(pImageData[y * qImage.width() + x]).toHsl();
			if (abs(mColor.saturation() - mBGColorBase.saturation()) > 20)
			{
				bDiff = true;
				break;
			}
		}

		if (bInRange)
		{
			if (!bDiff)
			{
				iRowEnd = y;
				break;
			}
		}
		else
		{
			if (bDiff)
			{
				bInRange = true;
				iRowBegin = y;
			}
		}
	}

	if (iRowEnd > iRowBegin )
		return QRect(qRange.left(), iRowBegin, qRange.right(), iRowEnd - iRowBegin);
	else
		return QRect();
}

QRect detectColumn(const QImage& qImage, const QColor& mBGColorBase, const QRect& qRange)
{
	const QRgb* pImageData = (const QRgb*)(qImage.constBits());

	int iLeft = -1, iRight = -2;
	bool bInRange = false;

	for (int x = qRange.left(); x < qRange.right(); ++x)
	{
		bool bDiff = false;
		for (int y = qRange.top(); y < qRange.bottom(); ++y)
		{
			QColor mColor = QColor::fromRgba(pImageData[y * qImage.width() + x]).toHsl();
			if (abs(mColor.saturation() - mBGColorBase.saturation()) > 20)
			{
				bDiff = true;
				break;
			}
		}

		if (bInRange)
		{
			if (!bDiff)
			{
				iRight = x;
				break;
			}
		}
		else
		{
			if (bDiff)
			{
				bInRange = true;
				iLeft = x;
			}
		}
	}

	if (iRight > iLeft)
		return QRect(iLeft, qRange.top(), iRight - iLeft, qRange.height());
	else
		return QRect();
}
#pragma endregion

SAOMDListImage::SAOMDListImage(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	ui.graphicsView->setScene(new QGraphicsScene());
	setAcceptDrops(true);
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
	for (const QString& sFile : aFileList)
	{
		processFile(sFile);
	}
	updateLayout();
}

#pragma region slot functions
void SAOMDListImage::slotLoad()
{
	QStringList aFiles = QFileDialog::getOpenFileNames( this, tr("Select one or more files"), ".", "Images (*.png *.jpg);;Any files (*.*)");
	if (aFiles.size() > 0)
		loadFiles(aFiles);
}

void SAOMDListImage::slotSave()
{
	QString sFile = QFileDialog::getSaveFileName(this, tr("Input the file to save"), ".", "Images (*.png)");
	if (!sFile.isEmpty())
	{
		QImage image(ui.graphicsView->scene()->sceneRect().size().toSize(), QImage::Format_ARGB32);
		image.fill(Qt::transparent);

		QPainter painter(&image);
		ui.graphicsView->scene()->render(&painter);
		image.save(sFile);
	}
}
#pragma endregion

bool SAOMDListImage::processFile(const QString& sFilename)
{
	QImage qImage;
	if (qImage.load(sFilename))
	{
		qImage = qImage.convertedTo(QImage::Format_ARGB32);

		int iLeft = 0, iTop = 0;
		QColor mBGColorBase;

		// detect background color
		const QRgb* pImageData = (const QRgb*)(qImage.constBits());
		int m = qImage.height() / 2,
			w = qImage.width();
		int iShift = w * m;
		for (int x = 0; x < w; ++x)
		{
			QColor mColor = QColor::fromRgba(pImageData[iShift + x]).toHsl();

			if (mColor.saturation() < 10)
			{
				iLeft = x + 2;
				mBGColorBase = QColor::fromRgba(pImageData[iShift + iLeft]).toHsl();
				auto l1 = mBGColorBase.lightness();
				break;
			}
		}

		QRect qRange(iLeft, iTop, w - iLeft, qImage.height() - iTop);
		while (true)
		{
			QRect rowRect = detectRow(qImage, mBGColorBase, qRange);
			if (rowRect.isValid())
			{
				qRange.setTop(rowRect.bottom() + 2);

				if (rowRect.height() > 50)
				{
					while (true)
					{
						QRect rowBox = detectColumn(qImage, mBGColorBase, rowRect);
						if (rowBox.isValid())
						{
							rowRect.setLeft(rowBox.right() + 2);

							if (rowBox.width() > 50)
							{
								float fRatio = (float)rowBox.height() / rowBox.width();
								if (fRatio > 1.1f && fRatio < 1.2f)
								{
									ui.graphicsView->scene()->addPixmap(QPixmap::fromImage(qImage.copy(rowBox)));
								}
							}
						}
						else
						{
							break;
						}
					}
				}
			}
			else
			{
				break;
			}
		}

		//ui.graphicsView->scene()->addPixmap(QPixmap::fromImage(qImage));
		return true;
	}
	else
	{
		QMessageBox::warning(this, tr("Load fail"), tr("File load fail: ") + sFilename);
		return false;
	}
}

void SAOMDListImage::updateLayout()
{
	int iCol = ui.hsColumnNum->value();
	int iBorder = ui.hsItemBorder->value();
	auto pScene = ui.graphicsView->scene();
	auto vItems = pScene->items(Qt::AscendingOrder);

	int x = 0, y = 0, num = 0;
	for (auto& pItem : vItems)
	{
		pItem->setPos(x, y);
		x += (iBorder + pItem->boundingRect().width() );

		if (++num > iCol)
		{
			num = 0;
			x = 0;
			y += (pItem->boundingRect().height() + iBorder);
		}
	
	}

	pScene->setSceneRect(pScene->itemsBoundingRect());
	ui.graphicsView->fitInView(pScene->sceneRect(),Qt::KeepAspectRatio);
}
