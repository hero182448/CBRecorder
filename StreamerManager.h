#ifndef STREAMERMANAGER_H
#define STREAMERMANAGER_H

#include <QObject>
#include <QVector>

#include "Streamer.h"

class StreamerManager : public QObject
{
        Q_OBJECT

    public:
        static StreamerManager* getInstance();
        static void deleteInstance();

        void initialize();

        void addStreamer(const QString& name);
        void deleteStreamer(Streamer* streamer);

        void updateRecordASAP();

        void setShutdown(bool shutdown);

    signals:
        void streamerAdded(Streamer* streamer);
        void streamerDeleted(Streamer* streamer);

    public slots:

    private slots:
        void onRecordingChanged(bool status);
        void onRecordASAPTimerTimeout();

    protected:
        explicit StreamerManager(QObject* parent = nullptr);

    private:
        bool isRecordingAnyone();

        void addModel(const QString& string);
        QStringList readModels();
        void removeModel(const QString& string);

    private:
        static StreamerManager* m_instance;

        QVector<Streamer*> m_streamers;
        bool m_shutdown;

        QTimer* m_recordingASAPTimer;
        bool m_recordingASAP;
};

#endif // STREAMERMANAGER_H
