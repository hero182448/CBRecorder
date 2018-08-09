#include "HttpRequest.h"

HttpRequest::HttpRequest(const QString& url, const QString& httpMethod)
{
    m_url = url;
    m_httpMethod = httpMethod;

    m_layout = LAYOUT_NOT_SET;
}

const QString& HttpRequest::httpMethod()
{
    return m_httpMethod;
}

const QString& HttpRequest::url()
{
    return m_url;
}

void HttpRequest::setUrl(const QString& url)
{
    m_url = url;
}

QList<HttpRequestInputFileElement>& HttpRequest::files()
{
    return m_files;
}

void HttpRequest::addFile(const QString& variableName, const QString& localFilename, const QString& requestFilename, const QString& mimeType)
{
    HttpRequestInputFileElement file;
    file.variableName = variableName;
    file.localFilename = localFilename;
    file.requestFilename = requestFilename;
    file.mimeType = mimeType;

    m_files.append(file);
}

QString HttpRequest::parameter(const QString& key)
{
    return m_parameters.value(key, "");
}

QMap<QString, QString> HttpRequest::parameters()
{
    return m_parameters;
}

void HttpRequest::addParameter(const QString& key, const QString& value)
{
    m_parameters.insert(key, value);
}

HttpRequestVarLayout HttpRequest::layout()
{
    return m_layout;
}

void HttpRequest::setLayout(HttpRequestVarLayout layout)
{
    m_layout = layout;
}
