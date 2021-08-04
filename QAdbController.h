#pragma once

#include <QProcess>
#include <QThread>
#include <QSize>
#include <QImage>
#include <QTextStream>

class QAdbController
{
public:
	QAdbController()
	{
		QObject::connect(&m_qProc, &QProcess::errorOccurred, [](QProcess::ProcessError e) {
		});
	}

	QStringList listDevices()
	{
		return runCmd(QStringList() << "devices", [](const QByteArray& aData) {
			QStringList vList;
			QTextStream isRes(aData);

			// read first line as header
			QString sLine = isRes.readLine();
			QString sDevice;

			while (isRes.readLineInto(&sLine))
			{
				if (!sLine.isEmpty())
				{
					QTextStream isLine(&sLine);
					isLine >> sDevice;
					vList.push_back(sDevice);
				}
			}

			return vList;
		});
	}

	QImage getScreen()
	{
		screenCapture();
		return runCmd(QStringList() << "pull" << m_sDeviceTmpImage << m_sLocalTmpImage, [sTmpName = m_sLocalTmpImage](const QByteArray& aData) {
			return QImage(sTmpName);
		});
	}

	void saveScreen(const QString& sName)
	{
		screenCapture();
		return runCmd(QStringList() << "pull" << m_sDeviceTmpImage << sName, [](const QByteArray& aData) {
			return;
		});
	}

	void swipe(QPoint vStart, QPoint vEnd, int iTime)
	{
		runCmd(QStringList() << "shell" << "input" << "swipe"
			<< QString::number(vStart.x()) << QString::number(vStart.y())
			<< QString::number(vEnd.x()) << QString::number(vEnd.y())
			<< QString::number(iTime));
	}

	void tap(QPoint vPos)
	{
		runCmd(QStringList() << "shell" << "input" << "tap"
			<< QString::number(vPos.x()) << QString::number(vPos.y()));
	}

private:
	template<typename TFUNC>
	auto runCmd(const QStringList& vArgs, TFUNC func)
	{
		m_qProc.start(m_sAdbCmd, vArgs);
		if (m_qProc.waitForStarted() && m_qProc.waitForFinished())
		{
			return func(m_qProc.readAll());
		}
	}

	void runCmd(const QStringList& vArgs)
	{
		runCmd(vArgs, [](const QByteArray&) { return; });
	}

	void screenCapture()
	{
		runCmd(QStringList() << "shell" << "screencap" << m_sDeviceTmpImage, [](const QByteArray&) { return; });
	}

private:
	QString		m_sAdbCmd	= "adb.exe";
	QProcess	m_qProc;
	QSize		m_qScreenSize;

	QString		m_sDeviceTmpImage	= "/sdcard/screen.png";
	QString		m_sLocalTmpImage	= "fromDevice.png";
};
