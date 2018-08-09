#ifndef STREAMER_H
#define STREAMER_H

#include <QObject>
#include <QProcess>
#include <QThread>
#include <QTimer>

#include "HttpRequestManager.h"
#include "StreamRecorder.h"

class Streamer : public QObject
{
    Q_OBJECT

    public:
        explicit Streamer(const QString& name, QObject *parent = nullptr);

        bool isAvailable() const;
        void checkAvailability();

        void startStream();
        void stopStream();

        bool isRecordASAP();
        void setRecordASAP(bool status);

        QString getName() const;
        QString getM3u8() const;
        QString getUrl() const;

        bool isRecording() const;

    signals:
        void startStream_sig();

        void availabilityChanged(bool available);
        void recordingChanged(bool status);

    public slots:

    private slots:
        void onCheckFinished();

        void onThreadStarted();
        void onRecordingChanged(bool status);

        void onRecordingTimerTimeout();

    private:
        QString m_name;

        QString m_m3u8;
        QString m_url;

        bool m_available;
        bool m_recording;
        bool m_recordingStoppedByUser;
        bool m_recordASAP;

        StreamRecorder* m_streamRecorder;
        QThread* m_thread;

        QTimer* m_recordingTimer;
};

#endif // STREAMER_H
