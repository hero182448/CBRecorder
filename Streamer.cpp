#include "Streamer.h"

#include <QTimer>

#include "MainWindow.h"

Streamer::Streamer(const QString& name, QObject *parent) : QObject(parent)
{
    m_name = name;
    m_url = "https://it.chaturbate.com/" + m_name + "/";

    m_available = false;
    m_recording = false;
    m_recordingStoppedByUser = false;
    m_recordASAP = false;

    m_thread = new QThread();
    m_streamRecorder = new StreamRecorder();
    QObject::connect(m_streamRecorder, SIGNAL(recordingChanged(bool)), SLOT(onRecordingChanged(bool)));
    QObject::connect(m_streamRecorder, SIGNAL(recordingChanged(bool)), SIGNAL(recordingChanged(bool)));

    m_streamRecorder->moveToThread(m_thread);
    QObject::connect(m_thread, SIGNAL(started()), SLOT(onThreadStarted()));
    QObject::connect(m_thread, SIGNAL(finished()), SLOT(deleteLater()));

    m_recordingTimer = new QTimer();
    m_recordingTimer->setInterval(360 * 60 * 1000); //6 hours
    m_recordingTimer->setSingleShot(true);
    QObject::connect(m_recordingTimer, SIGNAL(timeout()), SLOT(onRecordingTimerTimeout()));

    QObject::connect(this, SIGNAL(startStream_sig()), m_streamRecorder, SLOT(startStream()));
}

bool Streamer::isAvailable() const
{
    return m_available;
}

void Streamer::checkAvailability()
{
    HttpRequest input(m_url, "GET");

    HttpRequestWorker* worker = new HttpRequestWorker();
    QObject::connect(worker, SIGNAL(requestFinished()), this, SLOT(onCheckFinished()));
    worker->execute(input);
}

void Streamer::startStream()
{
    m_recordingStoppedByUser = false;

    m_streamRecorder->setName(m_name);
    m_streamRecorder->setM3u8(m_m3u8);

    if(!m_thread->isRunning())
    {
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

bool Streamer::isRecordASAP()
{
    return m_recordASAP;
}

void Streamer::setRecordASAP(bool status)
{
    m_recordASAP = status;

    if(status && isAvailable())
    {
        startStream();
    }
}

void Streamer::onCheckFinished()
{
    HttpRequestWorker* worker = qobject_cast<HttpRequestWorker*>(QObject::sender());

    QString response = worker->getResponse();
    if(response.contains("m3u8"))
    {
        if(!m_available)
        {
            MainWindow::getInstance()->showNotification(m_name + " has connected");

            m_available = true;

            if(m_recordASAP)
            {
                startStream();
            }
        }

        response = response.left(response.indexOf("m3u8") + 4);
        response = response.mid(response.lastIndexOf("http"));
        m_m3u8 = response;
    }
    else
    {
        m_available = false;
    }

    worker->deleteLater();

    emit availabilityChanged(m_available);

    QTimer::singleShot(60000, [=] () {
        checkAvailability();
    });
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

        m_recordingTimer->start();
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

void Streamer::onRecordingTimerTimeout()
{
    qDebug() << Q_FUNC_INFO << "Timer timed out, stopping stream";
    m_streamRecorder->stopStream();
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

bool Streamer::isRecording() const
{
    return m_recording;
}
