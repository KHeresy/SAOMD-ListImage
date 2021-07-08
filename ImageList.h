#pragma once

#include <QImage>

class CImageList
{
public:
	QImage			m_qImage;
	QList<QRect>	m_vRects;

public:
	CImageList(const QString& sFilename);

	QImage getItem(const QRect& qRect);

	bool isVaild() const
	{
		return !m_qImage.isNull();
	}

	int size() const
	{
		return m_vRects.size();
	}

protected:
	QRgb	m_BackgroundColor;
};
