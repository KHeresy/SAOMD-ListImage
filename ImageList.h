#pragma once

#include <QImage>

class CImageList
{
public:
	QImage			m_qImage;
	QList<QRect>	m_vRects;
	QList<QImage>	m_vItems;

public:
	CImageList(const QString& sFilename);

	bool isVaild() const
	{
		return !m_qImage.isNull();
	}

	int size() const
	{
		return m_vRects.size();
	}

protected:
	QImage getItem(const QRect& qRect);

protected:
	QRgb	m_BackgroundColor;
};
