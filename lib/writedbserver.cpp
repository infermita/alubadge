#include "writedbserver.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>
#include "lib/dao.h"
#include "lib/httpclient.h"

WriteDbServer::WriteDbServer()
{

}
void WriteDbServer::run(){

    qDebug() << "Write db on server";
    //sleep(62);
    Dao d;
    QJsonArray arr;
    QJsonObject obj;
    HttpClient http;
    bool res = false;

    while(!res){

        QList< QHash<QString, QString> > resQ = d.listRow("giorni","*","");
        QHash<QString, QString>::const_iterator iF;

        for(int i = 0; i < resQ.length();i++){
            QJsonObject tmp;
            //tmp.insert( resQ.at(i));
            for (iF = resQ.at(i).constBegin(); iF != resQ.at(i).constEnd(); ++iF){
                if(iF.key()!="id")
                    tmp.insert(iF.key(),iF.value());
            }

            arr.append(tmp);

        }
        obj [ "list" ] = arr;

        QJsonDocument doc ( obj );

        res = http.Post("/default/json/getdbbadge",doc.toJson());

        if(res){

            d.deleteRow("giorni","1");
            QStringList sets;
            sets.append("seq='0'");
            d.updateRow("sqlite_sequence",sets,"name='giorni'");

        }

    }

    qDebug() << "Finito Write db on server";

}
