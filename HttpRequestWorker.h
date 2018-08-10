#ifndef HTTPREQUESTWORKER_H
#define HTTPREQUESTWORKER_H

#include <QBuffer>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QNetworkReply>
#include <QObject>

#include "HttpRequest.h"

class HttpRequestWorker : public QObject
{
    Q_OBJECT

    public:
        explicit HttpRequestWorker();

        QString encodeAttribute(QString attribute, QString input);
        void execute(HttpRequest input);

        QByteArray getResponse() const;
        QImage getThumbnail() const;

    signals:
        void requestFinished(/*HttpRequestWorker* worker*/);

    private slots:
        void onRequestFinished(QNetworkReply* reply);

    private:
        QNetworkAccessManager* m_manager;

        HttpRequest m_input;

        QNetworkReply::NetworkError m_networkError;
        QString m_error;

        QByteArray m_response;
};

#endif // HTTPREQUESTWORKER_H
