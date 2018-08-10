#ifndef STREAMER_H
#define STREAMER_H

#include <QObject>
#include <QThread>
#include <QTimer>

#include "HttpRequestManager.h"
#include "StreamRecorder.h"

class Streamer : public QObject
{
    Q_OBJECT

    public:
        explicit Streamer(const QString& name, QObject *parent = nullptr);

        void startStream();
        void stopStream();
        bool isRecording() const;

        void setRecordASAP(bool status);
        bool isRecordASAP() const;

        bool isOnline() const;
        QString getName() const;
        QString getM3u8() const;
        QString getUrl() const;
        const QImage& getThumbnail() const;

    signals:
        void startStream_sig();

        void availabilityChanged(bool available);
        void recordingChanged(bool status);
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
        bool m_recordingStoppedByUser;
        bool m_recordASAP;

        StreamRecorder* m_streamRecorder;
        QThread* m_thread;
};

#endif // STREAMER_H
