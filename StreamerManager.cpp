#include "StreamerManager.h"

#include <QDebug>
#include <QSettings>

StreamerManager* StreamerManager::m_instance = nullptr;

StreamerManager::StreamerManager(QObject* parent) : QObject(parent)
{
    m_shutdown = false;

    m_recordingASAPTimer = new QTimer(this);
    m_recordingASAPTimer->setInterval(60 * 60 * 1000); //1h
    QObject::connect(m_recordingASAPTimer, &QTimer::timeout, this, &StreamerManager::onRecordASAPTimerTimeout);

    m_recordingASAP = false;
}

StreamerManager* StreamerManager::getInstance()
{
    if(!m_instance)
    {
        m_instance = new StreamerManager();
    }

    return m_instance;
}

void StreamerManager::deleteInstance()
{
    if(m_instance)
    {
        delete m_instance;
    }
}

void StreamerManager::initialize()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    quint32 size = settings.beginReadArray("Models");
    for(quint32 i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        addStreamer(settings.value("Model").toString());
    }
    settings.endArray();
}

void StreamerManager::addStreamer(const QString& name)
{
    foreach(Streamer* i_streamer, m_streamers)
    {
        if(i_streamer->getName() == name)
        {
            return;
        }
    }

    qDebug() << "Adding model" << name;
    addModel(name);

    Streamer* streamer = new Streamer(name);
    QObject::connect(streamer, &Streamer::recordingChanged, this, &StreamerManager::onRecordingChanged);
    QObject::connect(streamer, &Streamer::retryingToRecordChanged, this, &StreamerManager::onRetryingToRecordChanged);
    m_streamers.push_back(streamer);

    emit streamerAdded(streamer);
}

void StreamerManager::deleteStreamer(Streamer* streamer)
{
    qDebug() << "Removing model" << streamer->getName();
    removeModel(streamer->getName());

    m_streamers.removeOne(streamer);
    streamer->deleteLater();
}

void StreamerManager::updateRecordASAP()
{
    bool newStatus = false;
    foreach(Streamer* i_streamer, m_streamers)
    {
        if(i_streamer->isRecordASAP())
        {
            newStatus = true;
            break;
        }
    }

    if(newStatus && !m_recordingASAP)
    {
        qDebug() << "Starting m_recordingASAPTimer";
        m_recordingASAP = true;
        m_recordingASAPTimer->start();
    }
    else if(!newStatus && m_recordingASAP)
    {
        qDebug() << "Stopping m_recordingASAPTimer";
        m_recordingASAP = false;
        m_recordingASAPTimer->stop();
    }
}

void StreamerManager::setShutdown(bool shutdown)
{
    qDebug() << (shutdown ? "Shutting down PC" : "Not shutting down PC");
    m_shutdown = shutdown;
}

void StreamerManager::onRecordingChanged(bool status)
{
    Streamer* streamer = qobject_cast<Streamer*>(QObject::sender());

    qDebug() << "status" << status << "isRecordingAnyone" << isRecordingAnyone() << "streamer->isRetryingToRecord()" << streamer->isRetryingToRecord() << "m_shutdown" << m_shutdown;
    if(!status && !isRecordingAnyone() && !streamer->isRetryingToRecord() && m_shutdown)
    {
        Utilities::getInstance()->startShutdownTimer();
    }
}

void StreamerManager::onRetryingToRecordChanged(bool status)
{
    qDebug() << "status" << status << "isRecordingAnyone" << isRecordingAnyone() << "isRetryingToRecordAnyone()" << isRetryingToRecordAnyone() << "m_shutdown" << m_shutdown;
    if(!status && !isRecordingAnyone() && !isRetryingToRecordAnyone() && m_shutdown)
    {
        Utilities::getInstance()->startShutdownTimer();
    }
}

void StreamerManager::onRecordASAPTimerTimeout()
{
    qDebug() << "isRecordingAnyone" << isRecordingAnyone();
    if(!isRecordingAnyone())
    {
        Utilities::getInstance()->startShutdownTimer();
    }
    else
    {
        //set shutdown to true so that the computer will shutdown as soon as recording finishes
        setShutdown(true);
    }
}

bool StreamerManager::isRecordingAnyone()
{
    foreach(Streamer* i_streamer, m_streamers)
    {
        if(i_streamer->isRecording())
        {
            return true;
        }
    }

    return false;
}

bool StreamerManager::isRetryingToRecordAnyone()
{
    foreach(Streamer* i_streamer, m_streamers)
    {
        if(i_streamer->isRetryingToRecord())
        {
            return true;
        }
    }

    return false;
}

void StreamerManager::addModel(const QString& string)
{
    QStringList models = readModels();
    if(!models.contains(string))
    {
        models << string;

        QSettings settings("settings.ini", QSettings::IniFormat);
        settings.beginWriteArray("Models");
        for(qint32 i = 0; i < models.size(); ++i)
        {
            settings.setArrayIndex(i);
            settings.setValue("Model", models.at(i));
        }
        settings.endArray();
    }
}

QStringList StreamerManager::readModels()
{
    QStringList models;

    QSettings settings("settings.ini", QSettings::IniFormat);
    quint32 size = settings.beginReadArray("Models");
    for(quint32 i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        models << settings.value("Model").toString();
    }
    settings.endArray();

    return models;
}

void StreamerManager::removeModel(const QString& string)
{
    QStringList models = readModels();
    models.removeOne(string);

    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.beginWriteArray("Models");
    for(qint32 i = 0; i < models.size(); ++i)
    {
        settings.setArrayIndex(i);
        settings.setValue("Model", models.at(i));
    }
    settings.endArray();
}
