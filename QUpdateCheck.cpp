#include "QUpdateCheck.h"
#include "Define.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

QUpdateCheck::QUpdateCheck(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);

	ui.labelVersion->setText(SQOMDLI_VER);
	ui.labelReleaseDate->setText(SAOMDLI_RELEASE);
}

void QUpdateCheck::slotCheckUpdate()
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

void QUpdateCheck::requestFinished(QNetworkReply* qReply)
{
	ui.pbCheck->setDisabled(false);
	ui.pbCheck->setText(tr("Check Update"));

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
				QString sDate = qRelease["created_at"].toString().mid(0,10);
				QString sContent = qRelease["body"].toString();

				if (!sTagName.isEmpty() && !sDate.isEmpty())
				{
					QJsonArray qAssets = qRelease["assets"].toArray();
					if (qAssets.size() == 1)
					{
						QString sLink = qAssets[0].toObject()["browser_download_url"].toString();

						return;
					}
				}
			}
		}
	}
	QMessageBox::warning(this, tr("Check Update"), tr("Check failed: return syntax error"), QMessageBox::Close);
}
