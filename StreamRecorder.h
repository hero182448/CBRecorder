#ifndef STREAMRECORDER_H
#define STREAMRECORDER_H

#include <QObject>
#include <QProcess>

class StreamRecorder : public QObject
{
        Q_OBJECT

    public:
        explicit StreamRecorder(QObject* parent = nullptr);

        void setName(const QString& name);
        void setM3u8(const QString& m3u8);

        QString getFilename() const;

    signals:
        void recordingChanged(bool status);

    public slots:
        void startRecording();
        void stopRecording();

    private slots:
        void onStarted();
        void onFinished(int exitCode, QProcess::ExitStatus exitStatus);

    private:
        static void createDirectories();

    private:
        QProcess* m_process;

        QString m_name;
        QString m_m3u8;

        QString m_filename;
};

#endif // STREAMRECORDER_H
