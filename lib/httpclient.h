#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H


#include <QString>
#include <QObject>

class HttpClient
{
public:
    HttpClient();
    QString Get(QString url);
    bool Post(QString url,QByteArray data);
private:
    bool lock;
};
#endif // HTTPCLIENT_H
