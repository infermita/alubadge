#include "writedbserver.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>
#include <QDateTime>
#include "lib/dao.h"
#include "lib/httpclient.h"

WriteDbServer::WriteDbServer(Dao *dObj)
{
    d = dObj;
}
void WriteDbServer::run(){

    qDebug() << "Write db on server";
    //sleep(62);
    //Dao d;
    QJsonArray arr;
    QJsonObject obj;
    HttpClient http;
    bool res = false;

    while(!res){

        QString dtSc = QDateTime::currentDateTime().addDays(-1).toString("yyyy-MM-dd");

        QList< QHash<QString, QString> > resQ = d->listRow("giorni","*","data='"+dtSc+"'");
        QHash<QString, QString>::const_iterator iF;

        arr = QJsonArray();

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

            d->deleteRow("giorni","data='"+dtSc+"'");
            if(QDateTime::currentDateTime().date().dayOfWeek()==7){
                QStringList sets;
                sets.append("seq='0'");
                d->updateRow("sqlite_sequence",sets,"name='giorni'");
            }

        }else{
            sleep(10);
        }

    }

    qDebug() << "Finito Write db on server";

}
