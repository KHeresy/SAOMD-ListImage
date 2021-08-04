#pragma once

#include <QImage>

class CImageList
{
public:
	QImage			m_qImage;
	QList<QList<QRect>>		m_vRects;
	QList<QList<QImage>>	m_vItems;

public:
	CImageList(const QString& sFilename);

	bool isVaild() const
	{
		return !m_qImage.isNull();
	}

	int size() const
	{
		int iSum = 0;
		for (auto& row : m_vRects)
			iSum += row.size();
		return iSum;
	}

	bool isFullRow() const
	{
		for (auto& row : m_vRects)
			if (row.size() != 4)
				return false;

		return true;
	}

protected:
	QImage getItem(const QRect& qRect);

protected:
	QRgb	m_BackgroundColor;
};
