#include "httpclient.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QEventLoop>
#include <QNetworkReply>
#include <QDebug>
#include <unistd.h>

HttpClient::HttpClient()
{
    lock = false;

}
QString HttpClient::Get(QString url){

    QNetworkAccessManager manager;
    QEventLoop loop;
    QNetworkReply *rep;
    QString urlDef;

    while(lock){

        qDebug() << "Attendo Richiamo http";
        qDebug() << "al url " << url;
        sleep(1);

    }

    if(lock==false){

        lock = true;

        urlDef = "http://alucount.al.it"+url;

        QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)),&loop, SLOT(quit()));
        rep = manager.get(QNetworkRequest(QUrl(urlDef)));

        qDebug() << "HttpC Chiamo url: " << urlDef;

        loop.exec();

        QVariant statusCode = rep->attribute( QNetworkRequest::HttpStatusCodeAttribute );

        qDebug() << "HttpC risposta: " << statusCode.toString();

        lock = false;

        if(statusCode.toInt()==200){

            return rep->readAll();

        }else{
            return "";
        }

    }/*else{
        qDebug() << "Attendo Richiamo http";
        qDebug() << "al url " << url;
        sleep(1);
        Costant::http.Get(url);
    }*/

}
bool HttpClient::Post(QString url, QByteArray data){

    QNetworkAccessManager manager;
    QEventLoop loop;
    QNetworkReply *rep;
    bool ret = false;
    QUrl serviceUrl = QUrl("http://alucount.al.it"+url);

    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)),&loop, SLOT(quit()));
    QNetworkRequest request(serviceUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    rep = manager.post(request,data);
    qDebug() << "HttpC Chiamo url: " << serviceUrl.toString();

    loop.exec();

    QVariant statusCode = rep->attribute( QNetworkRequest::HttpStatusCodeAttribute );

    qDebug() << "HttpC risposta: " << statusCode.toString();

    lock = false;

    if(statusCode.toInt()==200){

        qDebug() << "Leggo: " << rep->readAll();

        ret = true;

    }

    return ret;

}
