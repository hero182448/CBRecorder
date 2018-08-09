#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QMap>
#include <QString>

enum HttpRequestVarLayout
{
    LAYOUT_NOT_SET,
    LAYOUT_ADDRESS,
    LAYOUT_MULTIPART,
    LAYOUT_URL_ENCODED
};

struct HttpRequestInputFileElement
{
        QString variableName;
        QString localFilename;
        QString requestFilename;
        QString mimeType;
};

class HttpRequest
{
    public:
        HttpRequest(const QString& url = "", const QString& httpMethod = "GET");

        const QString& httpMethod();

        const QString& url();
        void setUrl(const QString& url);

        QList<HttpRequestInputFileElement>& files();
        void addFile(const QString& variableName, const QString& localFilename, const QString& requestFilename, const QString& mimeType);

        QString parameter(const QString& key);
        QMap<QString, QString> parameters();
        void addParameter(const QString& key, const QString& value);

        HttpRequestVarLayout layout();
        void setLayout(HttpRequestVarLayout layout);

    private:
        QString m_httpMethod;
        QString m_url;

        QList<HttpRequestInputFileElement> m_files;
        QMap<QString, QString> m_parameters;

        HttpRequestVarLayout m_layout;
};

#endif// HTTPREQUEST_H
