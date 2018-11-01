#ifndef STREAMER_H
#define STREAMER_H

#include <QObject>
#include <QThread>
#include <QTimer>

#include "HttpRequestWorker.h"
#include "StreamRecorder.h"

class Streamer : public QObject
{
    Q_OBJECT

    public:
        explicit Streamer(const QString& name, QObject* parent = nullptr);

        void startRecording();
        void stopRecording();
        bool isRecording() const;
        bool isRetryingToRecord() const;

        void setRecordASAP(bool status);
        bool isRecordASAP() const;

        bool isOnline() const;
        QString getName() const;
        QString getM3u8() const;
        QString getUrl() const;
        const QImage& getThumbnail() const;

    signals:
        void startRecordingWorker();

        void statusChanged(bool available);
        void recordingChanged(bool status);
        void retryingToRecordChanged(bool status);
        void thumbnailChanged(const QImage& image);

    private slots:
        void onStatusRetrieved();
        void onThumbnailRetrieved();

        void onThreadStarted();
        void onRecordingChanged(bool status);

        void onThumbnailTimerTimeout();

    private:
        void retrieveStatus();
        void retrieveThumbnail();

        QString m_name;
        QString m_m3u8;
        QString m_url;

        QTimer* m_thumbnailTimer;
        QImage m_thumbnail;

        bool m_online;
        bool m_recording;
        bool m_recordASAP;

        bool m_recordingStoppedByUser;
        bool m_retryingToRecord;

        StreamRecorder* m_streamRecorder;
        QThread* m_thread;
};

#endif // STREAMER_H
