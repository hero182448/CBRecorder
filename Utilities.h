#ifndef UTILITIES_H
#define UTILITIES_H

#include <QObject>
#include <QTimer>

class Utilities : public QObject
{
        Q_OBJECT

    public:
        static Utilities* getInstance();
        static void deleteInstance();

        void startShutdownTimer();
        void stopShutdownTimer();

        void showNotification(QString message);

    signals:
        void shutdownTimerStarted();
        void shutdownTimerStopped();

    public slots:

    private slots:
        void onShutdownTimerTimeout();

    protected:
        explicit Utilities(QObject* parent = nullptr);

    private:

    private:
        static Utilities* m_instance;

        QTimer* m_shutdownTimer;
};

#endif // UTILITIES_H
