#pragma once

#include <QtWidgets/QMainWindow>
#include <QSettings>

#include "ui_SAOMDListImage.h"
#include "QAbout.h"
#include "QUpdateCheck.h"
#include "ImageList.h"

class SAOMDListImage : public QMainWindow
{
	Q_OBJECT

public:
	SAOMDListImage(QWidget* parent = Q_NULLPTR);
	~SAOMDListImage();

private:
	void loadFiles(const QStringList& aFileList);
	QSettings getSettings();

private:
	void dragEnterEvent(QDragEnterEvent* pEvent) override;
	void dropEvent(QDropEvent* pEvent) override;

private slots:
	void slotLoad();
	void slotClear();
	void slotSave();
	void slotDemoVideo();
	void slotAbout();
	void slotDeleteSelected();
	void slotSelectChanged();
	void updateLayout();

private:
	Ui::SAOMDListImageClass	ui;

	QAbout*			m_qAbout;
	QUpdateCheck*	m_qUpdateCheck;

	QList<QGraphicsPixmapItem*>	m_vItems;
	QList<CImageList*>			m_vImageList;
};
