#include "QUpdateCheck.h"
#include "Define.h"

#include <QDesktopServices>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

class CVersion
{
public:
	CVersion(QString sVerStr)
	{
		if (sVerStr.length() > 3)
		{
			// check first character
			if (sVerStr[0] == 'v' || sVerStr[0] == 'V')
				sVerStr = sVerStr.mid(1);

			// check qualifier
			for (int i = sVerStr.length() - 1; i > 0; --i)
			{
				if (sVerStr[i].isNumber())
				{
					if (i != sVerStr.length() - 1)
					{
						sQualifier = sVerStr.mid(i+1);
						sVerStr = sVerStr.mid(0, i+1);
					}
					break;
				}
			}

			int iPos = sVerStr.indexOf(".");
			if (iPos > 0)
			{
				iMajor = sVerStr.mid(0, iPos).toInt();
				iMinor = sVerStr.mid(iPos+1).toInt();
			}
		}
	}

	bool operator<(const CVersion& rVer) const
	{
		if (iMajor < rVer.iMajor)
			return true;
		else if (iMajor > rVer.iMajor)
			return false;

		if (iMinor < rVer.iMinor)
			return true;
		else if (iMinor > rVer.iMinor)
			return false;

		return sQualifier < rVer.sQualifier;
	}

protected:
	int		iMajor	= 0;
	int		iMinor	= 0;
	QString	sQualifier;
};

QUpdateCheck::QUpdateCheck(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);

	ui.labelVersion->setText(QString("%1 @%2").arg(SQOMDLI_VER).arg(SAOMDLI_RELEASE));
}

void QUpdateCheck::slotCheckUpdate()
{
	if (qUpdateUrl.isEmpty())
	{
		QNetworkRequest qRequest;
		QNetworkAccessManager* qManager = new QNetworkAccessManager(this);
		QObject::connect(qManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));

		QSslConfiguration config = QSslConfiguration::defaultConfiguration();
		config.setProtocol(QSsl::AnyProtocol);
		qRequest.setSslConfiguration(config);

		qRequest.setUrl(QUrl("https://api.github.com/repos/KHeresy/SAOMD-ListImage/releases"));
		QNetworkReply* qReply = qManager->get(qRequest);

		ui.pbCheck->setDisabled(true);
		ui.pbCheck->setText(tr("Checking"));
	}
	else
	{
		QDesktopServices::openUrl(qUpdateUrl);
	}
}

void QUpdateCheck::requestFinished(QNetworkReply* qReply)
{
	ui.pbCheck->setDisabled(false);

	QVariant statusCode = qReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	if (statusCode.isValid())
	{
		if (statusCode.toInt() != 200)
		{
			QMessageBox::warning(this, tr("Check Update"), (tr("Server check failed: ") + statusCode.toString()), QMessageBox::Close);
			return;
		}
	}

	//QVariant reason = qReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
	//if (reason.isValid())
	//	qDebug() << "reason=" << reason.toString();

	QNetworkReply::NetworkError err = qReply->error();
	if (err != QNetworkReply::NoError)
	{
		QMessageBox::warning(this, tr("Check Update"), (tr("Check failed: ") + qReply->errorString()), QMessageBox::Close);
	}
	else
	{
		QJsonDocument qDoc = QJsonDocument::fromJson(qReply->readAll());
		if (qDoc.isArray())
		{
			QJsonArray qRelSet = qDoc.array();
			if (qRelSet.size() > 0)
			{
				QJsonObject qRelease = qRelSet[0].toObject();
				QString sTagName = qRelease["tag_name"].toString();
				//QString sDate = qRelease["created_at"].toString().mid(0,10);
				QString sContent = qRelease["body"].toString();

				if (CVersion(SQOMDLI_VER) < CVersion(sTagName))
				{
					if (!sTagName.isEmpty())
					{
						QJsonArray qAssets = qRelease["assets"].toArray();
						if (qAssets.size() == 1)
						{
							QString sLink = qAssets[0].toObject()["browser_download_url"].toString();

							ui.labelResult->setText(QString(tr("Newer version %1 Found")).arg(sTagName));
							ui.labelResult->setToolTip(sContent);
							ui.pbCheck->setText(tr("Download"));
							qUpdateUrl = QUrl(sLink);
							return;
						}
					}
				}
				else
				{
					ui.labelResult->setText(tr("You have latest version"));
					ui.pbCheck->setText(tr("Check Update"));
					return;
				}
			}
		}
	}
	QMessageBox::warning(this, tr("Check Update"), tr("Check failed: return syntax error"), QMessageBox::Close);
}
