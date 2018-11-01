#include "Streamer.h"

#include <QTimer>

#include "MainWindow.h"

Streamer::Streamer(const QString& name, QObject* parent) : QObject(parent)
{
    m_name = name;
    m_url = "https://it.chaturbate.com/" + m_name + "/";

    m_online = false;
    m_recording = false;
    m_recordASAP = false;

    m_recordingStoppedByUser = false;
    m_retryingToRecord = false;

    m_thread = nullptr;

    m_thumbnailTimer = new QTimer(this);
    m_thumbnailTimer->setInterval(30000);
    m_thumbnailTimer->setSingleShot(true);
    QObject::connect(m_thumbnailTimer, &QTimer::timeout, this, &Streamer::onThumbnailTimerTimeout);

    retrieveStatus();
}

void Streamer::startRecording()
{
    m_recordingStoppedByUser = false;

    if(!m_thread)
    {
        m_streamRecorder = new StreamRecorder();
        m_streamRecorder->setName(m_name);
        m_streamRecorder->setM3u8(m_m3u8);
        QObject::connect(m_streamRecorder, &StreamRecorder::recordingChanged, this, &Streamer::onRecordingChanged);
        QObject::connect(m_streamRecorder, &StreamRecorder::recordingChanged, this, &Streamer::recordingChanged);
        QObject::connect(this, &Streamer::startRecordingWorker, m_streamRecorder, &StreamRecorder::startRecording);

        m_thread = new QThread();
        m_streamRecorder->moveToThread(m_thread);
        QObject::connect(m_thread, &QThread::started, m_streamRecorder, &StreamRecorder::startRecording);
        QObject::connect(m_thread, &QThread::finished, m_streamRecorder, &StreamRecorder::deleteLater);
        m_thread->start();
    }
    else
    {
        emit startRecordingWorker();
    }
}

void Streamer::stopRecording()
{
    m_recordingStoppedByUser = true;

    m_streamRecorder->stopRecording();
}

bool Streamer::isRecording() const
{
    return m_recording;
}

bool Streamer::isRetryingToRecord() const
{
    return m_retryingToRecord;
}

void Streamer::setRecordASAP(bool status)
{
    m_recordASAP = status;

    if(status && isOnline())
    {
        startRecording();
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

void Streamer::onStatusRetrieved()
{
    HttpRequestWorker* worker = qobject_cast<HttpRequestWorker*>(QObject::sender());

    QString response = worker->getResponse();
    if(response.contains("m3u8"))
    {
        if(!m_online)
        {
            m_online = true;

            Utilities::getInstance()->showNotification(m_name + " has connected");

            if(m_recordASAP || m_retryingToRecord)
            {
                startRecording();
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

    emit statusChanged(m_online);

    qDebug() << "Retrieving status of" << m_name << "in 2 minutes";
    QTimer::singleShot(120000, [=] () {
        qDebug() << "Retrieving status of" << m_name << "now";
        retrieveStatus();
    });
}

void Streamer::onThumbnailRetrieved()
{
    qDebug() << "Streamer::onThumbnailRetrieved()";
    HttpRequestWorker* worker = qobject_cast<HttpRequestWorker*>(QObject::sender());

    m_thumbnail = worker->getThumbnail();
    emit thumbnailChanged(m_thumbnail);
}

void Streamer::onThreadStarted()
{
    emit startRecordingWorker();
}

void Streamer::onRecordingChanged(bool status)
{
    m_recording = status;
    
    if(m_recording)
    {
        Utilities::getInstance()->showNotification("Recording " + m_name);
    }
    else
    {
        Utilities::getInstance()->showNotification("Stopped recording " + m_name);

        QFile::rename(("recordings\\tmp\\" + m_streamRecorder->getFilename()), "recordings\\" + m_streamRecorder->getFilename());

        if(!m_recordingStoppedByUser)
        {
            m_retryingToRecord = true;
            emit retryingToRecordChanged(m_retryingToRecord);

            QTimer::singleShot(60000, [this] () {
                m_retryingToRecord = false;
                emit retryingToRecordChanged(m_retryingToRecord);
            });
        }
    }
}

void Streamer::onThumbnailTimerTimeout()
{
    qDebug() << "Streamer::onThumbnailTimerTimeout()";
    retrieveThumbnail();
}

void Streamer::retrieveStatus()
{
    HttpRequest input(m_url, "GET");

    HttpRequestWorker* worker = new HttpRequestWorker();
    QObject::connect(worker, &HttpRequestWorker::requestFinished, this, &Streamer::onStatusRetrieved);
    worker->execute(input);
}

void Streamer::retrieveThumbnail()
{
    if(m_online)
    {
        HttpRequest input("https://roomimg.stream.highwebmedia.com/ri/" + m_name + ".jpg", "GET");

        HttpRequestWorker* worker = new HttpRequestWorker();
        QObject::connect(worker, &HttpRequestWorker::requestFinished, this, &Streamer::onThumbnailRetrieved);
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
