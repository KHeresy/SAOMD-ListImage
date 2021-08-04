#include "ImageList.h"

#pragma region Internal process functions
float colorDistance(const QColor& c1, const QColor& c2)
{
	float	fDR = c1.redF() - c2.redF(),
		fDG = c1.greenF() - c2.greenF(),
		fDB = c1.blueF() - c2.blueF();

	return sqrt(fDR * fDR + fDG * fDG + fDB * fDB);
}

float colorDistance(const QRgb& mColor1, const QRgb& mColor2)
{
	return colorDistance(QColor::fromRgba(mColor1).toRgb(), QColor::fromRgba(mColor2).toRgb());
}

template<typename _TYPE>
bool sameColor(const _TYPE& mColor1, const _TYPE& mColor2)
{
	return(colorDistance(mColor1, mColor2) < 0.25f);
}

QRect detectRow(const QImage& qImage, const QRgb& mBGColorBase, const QRect& qRange)
{
	const QRgb* pImageData = (const QRgb*)(qImage.constBits());

	int iRowBegin = -1, iRowEnd = -2;
	bool bInRange = false;
	for (int y = qRange.top(); y < qRange.bottom(); ++y)
	{
		bool bDiff = false;
		for (int x = qRange.left(); x < qRange.width() / 4; ++x)
		{
			if (!sameColor(mBGColorBase, pImageData[y * qImage.width() + x]))
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

	if (iRowEnd > iRowBegin)
		return QRect(qRange.left(), iRowBegin, qRange.right(), iRowEnd - iRowBegin);
	else
		return QRect();
}

QRect detectColumn(const QImage& qImage, const QRgb& mBGColorBase, const QRect& qRange)
{
	const QRgb* pImageData = (const QRgb*)(qImage.constBits());

	int iLeft = -1, iRight = -2;
	bool bInRange = false;

	for (int x = qRange.left(); x < qRange.right(); ++x)
	{
		bool bDiff = false;
		for (int y = qRange.top(); y < qRange.bottom(); ++y)
		{
			if (!sameColor(mBGColorBase, pImageData[y * qImage.width() + x]))
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

void removeBorder(QImage& qImage, const QRgb& mBGColorBase)
{
	QRgb* pImageData = (QRgb*)(qImage.bits());
	for (int y = 0; y < qImage.height(); ++y)
	{
		for (int x = 0; x < qImage.width(); ++x)
		{
			QRgb& rRgb = pImageData[y * qImage.width() + x];
			if (sameColor(rRgb, mBGColorBase))
				rRgb = qRgba(0, 0, 0, 0);
			else
				break;
		}

		for (int x = qImage.width() - 1; x >= 0; --x)
		{
			QRgb& rRgb = pImageData[y * qImage.width() + x];
			if (QColor::fromRgba(rRgb).alpha() == 0)
				break;

			if (sameColor(rRgb, mBGColorBase))
				rRgb = qRgba(0, 0, 0, 0);
			else
				break;
		}
	}
}
#pragma endregion

CImageList::CImageList(const QString& sFilename)
{
	if (m_qImage.load(sFilename))
	{
		m_qImage = m_qImage.convertedTo(QImage::Format_ARGB32);

		int iLeft = 0, iTop = 0;

		// detect background color
		const QRgb* pImageData = (const QRgb*)(m_qImage.constBits());
		int m = m_qImage.height() / 2,
			w = m_qImage.width();
		int iShift = w * m;
		for (int x = 0; x < w; ++x)
		{
			QColor mColor = QColor::fromRgba(pImageData[iShift + x]).toHsl();

			if (mColor.saturation() < 10)
			{
				iLeft = x + 2;
				m_BackgroundColor = pImageData[iShift + iLeft];
				break;
			}
		}

		QRect qRange(iLeft, iTop, w - iLeft, m_qImage.height() - iTop);
		while (true)
		{
			QRect rowRect = detectRow(m_qImage, m_BackgroundColor, qRange);
			if (rowRect.isValid())
			{
				qRange.setTop(rowRect.bottom() + 2);

				if (rowRect.height() > 50)
				{
					while (true)
					{
						QRect rowBox = detectColumn(m_qImage, m_BackgroundColor, rowRect);
						if (rowBox.isValid())
						{
							rowRect.setLeft(rowBox.right() + 2);

							if (rowBox.width() > 50)//&& rowBox.width() < qImage.width() / 5)
							{
								float fRatio = (float)rowBox.height() / rowBox.width();
								if (fRatio > 1.0f && fRatio < 1.2f)
								{
									m_vRects.push_back(rowBox);
									m_vItems.push_back(getItem(rowBox));
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
	}
}

QImage CImageList::getItem(const QRect& qRect)
{
	QImage qItem = m_qImage.copy(qRect);
	removeBorder(qItem, m_BackgroundColor);
	return qItem;
}
