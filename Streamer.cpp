#include "Streamer.h"

#include <QTimer>

#include "MainWindow.h"

Streamer::Streamer(const QString& name, QObject *parent) : QObject(parent)
{
    m_name = name;
    m_url = "https://it.chaturbate.com/" + m_name + "/";

    m_online = false;
    m_recording = false;
    m_recordingStoppedByUser = false;
    m_recordASAP = false;

    m_thread = nullptr;

    m_thumbnailTimer = new QTimer(this);
    m_thumbnailTimer->setInterval(30000);
    m_thumbnailTimer->setSingleShot(true);
    QObject::connect(m_thumbnailTimer, SIGNAL(timeout()), SLOT(onThumbnailTimerTimeout()));

    retrieveStatus();
}

void Streamer::startStream()
{
    m_recordingStoppedByUser = false;

    if(!m_thread)
    {
        m_streamRecorder = new StreamRecorder();
        m_streamRecorder->setName(m_name);
        m_streamRecorder->setM3u8(m_m3u8);
        QObject::connect(m_streamRecorder, SIGNAL(recordingChanged(bool)), SLOT(onRecordingChanged(bool)));
        QObject::connect(m_streamRecorder, SIGNAL(recordingChanged(bool)), SIGNAL(recordingChanged(bool)));
        QObject::connect(this, SIGNAL(startStream_sig()), m_streamRecorder, SLOT(startStream()));

        m_thread = new QThread();
        m_streamRecorder->moveToThread(m_thread);
        QObject::connect(m_thread, SIGNAL(started()), SLOT(onThreadStarted()));
        QObject::connect(m_thread, SIGNAL(finished()), SLOT(deleteLater()));
        m_thread->start();
    }
    else
    {
        emit startStream_sig();
    }
}

void Streamer::stopStream()
{
    m_recordingStoppedByUser = true;

    m_streamRecorder->stopStream();
}

bool Streamer::isRecording() const
{
    return m_recording;
}

void Streamer::setRecordASAP(bool status)
{
    m_recordASAP = status;

    if(status && isOnline())
    {
        startStream();
    }
}

bool Streamer::isRecordASAP() const
{
    return m_recordASAP;
}

bool Streamer::isOnline() const
{
    return m_online;
}

QString Streamer::getName() const
{
    return m_name;
}

QString Streamer::getM3u8() const
{
    return m_m3u8;
}

QString Streamer::getUrl() const
{
    return m_url;
}

const QImage& Streamer::getThumbnail() const
{
    return m_thumbnail;
}

void Streamer::retrieveStatus()
{
    HttpRequest input(m_url, "GET");

    HttpRequestWorker* worker = new HttpRequestWorker();
    QObject::connect(worker, SIGNAL(requestFinished()), this, SLOT(onStatusRetrieved()));
    worker->execute(input);
}

void Streamer::onStatusRetrieved()
{
    HttpRequestWorker* worker = qobject_cast<HttpRequestWorker*>(QObject::sender());

    QString response = worker->getResponse();
    if(response.contains("m3u8"))
    {
        if(!m_online)
        {
            m_online = true;

            MainWindow::getInstance()->showNotification(m_name + " has connected");

            if(m_recordASAP)
            {
                startStream();
            }

            retrieveThumbnail();
        }

        response = response.left(response.indexOf("m3u8") + 4);
        response = response.mid(response.lastIndexOf("http"));
        m_m3u8 = response;
    }
    else
    {
        m_online = false;
    }

    worker->deleteLater();

    emit availabilityChanged(m_online);

    QTimer::singleShot(60000, [=] () {
        retrieveStatus();
    });
}

void Streamer::onThumbnailRetrieved()
{
    HttpRequestWorker* worker = qobject_cast<HttpRequestWorker*>(QObject::sender());

    m_thumbnail = worker->getImage();
    emit thumbnailChanged(m_thumbnail);
}

void Streamer::onThreadStarted()
{
    emit startStream_sig();
}

void Streamer::onRecordingChanged(bool status)
{
    m_recording = status;
    
    if(m_recording)
    {
        MainWindow::getInstance()->showNotification("Recording " + m_name);
    }
    else
    {
        MainWindow::getInstance()->showNotification("Stopped recording " + m_name);

        QFile::rename(("recordings\\tmp\\" + m_streamRecorder->filename()), "recordings\\" + m_streamRecorder->filename());

        if(!m_recordingStoppedByUser)
        {
            qDebug() << "Start recording again because recording wasn't stopped by user";
            QTimer::singleShot(1000, [=] ()
            {
                startStream();
            });
        }
    }
}

void Streamer::onThumbnailTimerTimeout()
{
    retrieveThumbnail();
}

void Streamer::retrieveThumbnail()
{
    if(m_online)
    {
        HttpRequest input("https://roomimg.stream.highwebmedia.com/ri/" + m_name + ".jpg", "GET");

        HttpRequestWorker* worker = new HttpRequestWorker();
        QObject::connect(worker, SIGNAL(requestFinished()), this, SLOT(onThumbnailRetrieved()));
        worker->execute(input);

        if(!m_thumbnailTimer->isActive())
        {
            m_thumbnailTimer->start();
        }
    }
    else
    {
        m_thumbnailTimer->stop();
    }
}
