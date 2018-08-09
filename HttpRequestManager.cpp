#include "HttpRequestManager.h"

HttpRequestManager::HttpRequestManager()
{
    m_available = false;
}

bool HttpRequestManager::isAvailable() const
{
    return m_available;
}

void HttpRequestManager::checkAvailability(const QString& url)
{
    HttpRequest input(url, "GET");

    HttpRequestWorker* worker = new HttpRequestWorker();
    QObject::connect(worker, SIGNAL(requestFinished()), this, SLOT(onCheckFinished()));
    worker->execute(input);
}

void HttpRequestManager::onCheckFinished()
{
    HttpRequestWorker* worker = qobject_cast<HttpRequestWorker*>(QObject::sender());

    QString response = worker->getResponse();

    if(response.contains("m3u8"))
    {
        m_available = true;

        qDebug() << "Streamer is online, retrieving url";

        response = response.left(response.indexOf("m3u8") + 4);
        response = response.mid(response.lastIndexOf("http"));

        qDebug() << "Url is" << response;
    }
    else
    {
        m_available = false;

        qDebug() << "Streamer is offline";
    }

    worker->deleteLater();

    emit checkFinished();
}
