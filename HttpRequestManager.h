#ifndef HTTPREQUESTMANAGER_H
#define HTTPREQUESTMANAGER_H

#include <QObject>

#include "HttpRequest.h"
#include "HttpRequestWorker.h"

class HttpRequestManager : public QObject
{
      Q_OBJECT

    public:
        HttpRequestManager();

        bool isAvailable() const;
        void checkAvailability(const QString& url);

    signals:
        void checkFinished();

    private slots:
        void onCheckFinished();

    private:
        bool m_available;
};

#endif // HTTPREQUESTMANAGER_H
