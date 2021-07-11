#pragma once

#include <QGraphicsView>

class CScaleControlView : public QGraphicsView
{
	Q_OBJECT

public:
	enum class EScaleMode
	{
		FitImage,
		FitWidth,
		FitHeight,
		Full,
		Half,
		Custom
	};

public:
	CScaleControlView(QWidget* pParent) : QGraphicsView(pParent)
	{
	}

	void UpdateViewScale();

	EScaleMode getMode() const
	{
		return m_eMode;
	}

	float getScale() const
	{
		return m_fScale;
	}

public slots:
	void slotZoomFitImage()
	{
		setScaleMode(EScaleMode::FitImage);
	}

	void slotZoomFitWidth()
	{
		setScaleMode(EScaleMode::FitWidth);
	}

	void slotZoomFitHeight()
	{
		setScaleMode(EScaleMode::FitHeight);
	}

	void slotZoomScaleFull()
	{
		setScaleMode(EScaleMode::Full);
	}

	void slotZoomScaleHalf()
	{
		setScaleMode(EScaleMode::Half);
	}

	void slotZoomIn()
	{

	}

	void slotZoomOut()
	{

	}

	void setScaleMode(CScaleControlView::EScaleMode eMode);
	void setScale(float fScale);

signals:
	void signalModeChanged(CScaleControlView::EScaleMode);
	void signalScaleChanged(float);

protected:
	void resizeEvent(QResizeEvent*) override
	{
		UpdateViewScale();
	}

private:
	EScaleMode	m_eMode		= EScaleMode::FitImage;
	float		m_fScale = 1.0f;
};
