#include "CScaleControlView.h"

void CScaleControlView::UpdateViewScale()
{
	if (m_eMode == EScaleMode::FitImage ||
		m_eMode == EScaleMode::FitWidth ||
		m_eMode == EScaleMode::FitHeight)
	{
		setScaleMode(m_eMode);
	}
}

void CScaleControlView::setScaleMode(EScaleMode eMode)
{
	auto rectScene = scene()->sceneRect();
	auto rectView = rect();
	auto vCenter = mapToScene(rectView.width() / 2, rectView.height() / 2);
	float fScale = m_fScale;

	switch (eMode)
	{
	case EScaleMode::FitImage:
		fitInView(rectScene, Qt::KeepAspectRatio);
		if (rectView.width() / rectView.height() > rectScene.width() / rectScene.height())
			fScale = (float)rectView.height() / rectScene.height();
		else
			fScale = (float)rectView.width() / rectScene.width();
		break;

	case EScaleMode::FitWidth:
		fitInView(rectScene.left(), vCenter.y(), rectScene.right(), vCenter.y(), Qt::KeepAspectRatio);
		fScale = (float)rectView.width() / rectScene.width();
		break;

	case EScaleMode::FitHeight:
		fitInView(vCenter.x(), rectScene.top(), vCenter.x(), rectScene.bottom(), Qt::KeepAspectRatio);
		fScale = (float)rectView.height() / rectScene.height();
		break;

	case EScaleMode::Full:
		if (fScale != 1.0f)
		{
			resetTransform();
			fScale = 1.0f;
		}
		break;

	case EScaleMode::Half:
		if (fScale != 0.5f)
		{
			resetTransform();
			scale(0.5, 0.5);
			fScale = 0.5f;
		}
		break;
	}

	centerOn(vCenter);

	setScale(fScale);
	if (m_eMode != eMode)
	{
		m_eMode = eMode;
		emit signalModeChanged(m_eMode);
	}
}

void CScaleControlView::setScale(float fScale)
{
	if (fScale != m_fScale)
	{
		m_fScale = fScale;
		if (m_eMode == EScaleMode::Custom)
		{
			resetTransform();
			scale(m_fScale, m_fScale);

			auto rectView = rect();
			centerOn(mapToScene(rectView.width() / 2, rectView.height() / 2));
		}

		emit signalScaleChanged(m_fScale);
	}
}
