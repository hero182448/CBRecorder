#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>

#include "StreamerListView.h"
#include "StreamerManager.h"

class MainWindow : public QWidget
{
        Q_OBJECT

    public:
        static MainWindow* getInstance();

        void showNotification(QString message);

    protected:
        explicit MainWindow(QWidget* parent = nullptr);

    private slots:
        void onStreamerSelected(Streamer* streamer);
        void onStreamerRecordingChanged(bool recording);

        void onAddStreamerClicked();
        void onRecordStreamerClicked();

        void onShutdownClicked();

    private:
        static MainWindow* m_instance;

        QHBoxLayout* m_mainLayout;

        QVBoxLayout* m_leftLayout;
        QVBoxLayout* m_rightLayout;

        QLineEdit* m_streamerName;
        QPushButton* m_addStreamer;
        QSpacerItem* m_leftSpacer;

        StreamerListView* m_streamerList;
        QPushButton* m_recordStreamer;
        QPushButton* m_shutdown;
        QSpacerItem* m_rightSpacer;
};

#endif // MAINWINDOW_H
