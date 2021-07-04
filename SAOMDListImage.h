#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SAOMDListImage.h"
#include "QAbout.h"

class SAOMDListImage : public QMainWindow
{
	Q_OBJECT

public:
	SAOMDListImage(QWidget *parent = Q_NULLPTR);

private:
	bool processFile(const QString& sFilename);
	void loadFiles(const QStringList& aFileList);

private:
	void dragEnterEvent(QDragEnterEvent* pEvent) override;
	void dropEvent(QDropEvent* pEvent) override;

private slots:
	void slotLoad();
	void slotSave();
	void slotAbout();
	void updateLayout();

private:
	Ui::SAOMDListImageClass	ui;
	QAbout* m_qAbout;
	QList<QGraphicsPixmapItem*>	m_aItems;
};
