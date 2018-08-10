#include "MainWindow.h"

#include <QSystemTrayIcon>

MainWindow* MainWindow::m_instance = nullptr;

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    m_addStreamer = new QPushButton("Add");
    QObject::connect(m_addStreamer, SIGNAL(clicked(bool)), SLOT(onAddStreamerClicked()));

    m_thumbnail = new QLabel();
    m_thumbnail->setFixedSize(300, 300);

    m_streamerName = new QLineEdit();

    m_leftSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_leftLayout = new QVBoxLayout();
    m_leftLayout->addWidget(m_streamerName);
    m_leftLayout->addWidget(m_addStreamer);
    m_leftLayout->addWidget(m_thumbnail);
    m_leftLayout->addItem(m_leftSpacer);

    m_streamerList = new StreamerListView(true, this);
    QObject::connect(m_streamerList, SIGNAL(streamerSelected(Streamer*)), SLOT(onStreamerSelected(Streamer*)));

    m_recordStreamer = new QPushButton("Record");
    QObject::connect(m_recordStreamer, SIGNAL(clicked(bool)), SLOT(onRecordStreamerClicked()));

    m_shutdown = new QPushButton("Shutdown");
    QObject::connect(m_shutdown, SIGNAL(clicked(bool)), SLOT(onShutdownClicked()));

    m_rightSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_rightLayout = new QVBoxLayout();
    m_rightLayout->addWidget(m_streamerList);
    m_rightLayout->addWidget(m_recordStreamer);
    m_rightLayout->addWidget(m_shutdown);
    m_rightLayout->addItem(m_rightSpacer);

    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->addLayout(m_leftLayout);
    m_mainLayout->addLayout(m_rightLayout);

    setLayout(m_mainLayout);
}

MainWindow* MainWindow::getInstance()
{
    if(!m_instance)
    {
        m_instance = new MainWindow();
    }

    return m_instance;
}

void MainWindow::showNotification(QString message)
{
    QSystemTrayIcon notification;
    notification.setVisible(true);
    notification.showMessage("StreamRecorder", message);
}

void MainWindow::onStreamerSelected(Streamer* streamer)
{
    static Streamer* previousStreamer = nullptr;

    if(streamer)
    {
        if(previousStreamer)
        {
            QObject::disconnect(previousStreamer, SIGNAL(recordingChanged(bool)), this, SLOT(onStreamerRecordingChanged(bool)));
            QObject::disconnect(previousStreamer, SIGNAL(thumbnailChanged(QImage)), this, SLOT(onStreamerThumbnailChanged(QImage)));
        }

        previousStreamer = streamer;
        QObject::connect(streamer, SIGNAL(recordingChanged(bool)), SLOT(onStreamerRecordingChanged(bool)));
        QObject::connect(streamer, SIGNAL(thumbnailChanged(QImage)), this, SLOT(onStreamerThumbnailChanged(QImage)));

        m_recordStreamer->setText(streamer->isRecording() ? "Stop recording" : "Record");
        m_thumbnail->setPixmap(QPixmap::fromImage(streamer->getThumbnail()));
    }
}

void MainWindow::onStreamerRecordingChanged(bool recording)
{
    m_recordStreamer->setText(recording ? "Stop recording" : "Record");
}

void MainWindow::onStreamerThumbnailChanged(QImage thumbnail)
{
    m_thumbnail->setPixmap(QPixmap::fromImage(thumbnail));
}

void MainWindow::onAddStreamerClicked()
{
    StreamerManager::getInstance()->addStreamer(m_streamerName->text());
    //Maps the widget within its parent ---> m_streamerList->mapTo(this, m_streamerList->rect().topLeft());
}

void MainWindow::onRecordStreamerClicked()
{
    Streamer* streamer = m_streamerList->getSelectedStreamer();
    if(streamer)
    {
        if(streamer->isRecording())
        {
            streamer->stopStream();
        }
        else if(streamer->isOnline())
        {
            streamer->startStream();
        }
    }
}

void MainWindow::onShutdownClicked()
{
    if(m_shutdown->text() == "Shutdown")
    {
        m_shutdown->setText("Not shutdown");
        StreamerManager::getInstance()->setShutdown(true);
    }
    else
    {
        m_shutdown->setText("Shutdown");
        StreamerManager::getInstance()->setShutdown(false);
    }
}
