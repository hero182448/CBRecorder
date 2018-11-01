#include "Utilities.h"

#include <QDebug>
#include <QProcess>
#include <QSystemTrayIcon>

Utilities* Utilities::m_instance = nullptr;

Utilities::Utilities(QObject* parent) : QObject(parent)
{
    m_shutdownTimer = new QTimer(this);
    m_shutdownTimer->setInterval(180000);
    m_shutdownTimer->setSingleShot(true);
    QObject::connect(m_shutdownTimer, &QTimer::timeout, this, &Utilities::onShutdownTimerTimeout);
}

Utilities*Utilities::getInstance()
{
    if(!m_instance)
    {
        m_instance = new Utilities();
    }

    return m_instance;
}

void Utilities::deleteInstance()
{
    if(m_instance)
    {
        delete m_instance;
    }
}

void Utilities::startShutdownTimer()
{
    qDebug() << "Starting m_shutdownTimer...";
    if(!m_shutdownTimer->isActive())
    {
        m_shutdownTimer->start();
        emit shutdownTimerStarted();
    }
}

void Utilities::stopShutdownTimer()
{
    qDebug() << "Stopping m_shutdownTimer...";
    m_shutdownTimer->stop();
    emit shutdownTimerStopped();
}

void Utilities::showNotification(QString message)
{
    QSystemTrayIcon notification;
    notification.setVisible(true);
    notification.showMessage("StreamRecorder", message);
}

void Utilities::onShutdownTimerTimeout()
{
    qDebug() << "Shutting down PC in 60 seconds...";
    QProcess::startDetached("shutdown -s -f -t 60");
}
