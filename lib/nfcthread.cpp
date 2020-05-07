#include "nfcthread.h"
#include "lib/httpclient.h"
#include <QDebug>
#include <QNetworkInterface>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include <QDateTime>
#include <QJsonArray>
#include <QCoreApplication>
#include <QFile>

NfcThread::NfcThread()
{

    QTimer *viewDet;
    viewDet = new QTimer();
    connect(viewDet, SIGNAL(timeout()),
            this, SLOT(ViewData()),Qt::DirectConnection);
    viewDet->start(1000);
    writedb = 0;
    hour = QString(QCoreApplication::arguments().at(1)).toInt();


}
void NfcThread::run(){

    const nfc_modulation nmMifare = {
        .nmt = NMT_ISO14443A,
        .nbr = NBR_106,
    };
    QString id,url,lcd,ip,repeat = " ",resp;
    HttpClient http;
    int i;
    bool ipcheck = true;
    if(QString(getenv("USER"))!="alberto")
        wLcd = new WriteLcd();

    QJsonParseError *error = new QJsonParseError();
    QJsonDocument d;

    dao = new Dao();

    wdbserver = new WriteDbServer(dao);
    //wLcd->clear();
    //wLcd->write(0,0,"Attesa rete     ");
    WriteLcdT(0,0,"Attesa rete     ",true);

    qDebug() << "Scheduling sarà fatto alle " << hour;

    while(ipcheck){

        if(QString(getenv("USER"))=="alberto"){
            ip = QNetworkInterface::interfaceFromName("wlp5s0").addressEntries().first().ip().toString();
        }else{
            ip = QNetworkInterface::interfaceFromName("wlan0").addressEntries().first().ip().toString();
        }

        qDebug() << "Attesa rete con ip: " << ip << " count: " << QString::number(ip.split(".").count());

        if(ip.split(".").count()==4)
            ipcheck = false;
        else
            sleep(1);
    }

    url = "/default/json/getdbbadge/";

    resp = http.Get(url);

    d = QJsonDocument::fromJson(resp.toUtf8(),error);

    if(error->error==QJsonParseError::NoError){

        dao->deleteRow("cards","1");

        QJsonObject jObj = d.object();
        QJsonArray listUsers;


        listUsers = jObj["list"].toArray();
        foreach (const QJsonValue & users, listUsers) {

            field.clear();

            QJsonObject o = users.toObject();
            qDebug() << "Id user: " << o["id"].toString();
            field.insert("id",o["id"].toString());
            field.insert("cardkey",o["cardkey"].toString());
            field.insert("`name`",o["name_lastname"].toString());
            field.insert("`autocomplete`",o["autocomplete"].toString());

            if(dao->replaceRow("workers",field)){
                qDebug() << "Ok insert: " << o["id"].toString();
            }else{
                qDebug() << "Error insert: " << o["id"].toString();
            }

        }
    }

    //wLcd->clear();
    lcd = "Attesa badge";
    lcd = lcd+repeat.repeated(16 - lcd.length());
    //wLcd->write(0,0,lcd.toUtf8().data());
    WriteLcdT(0,0,lcd,true);
    vieData = 1;

    WriteDB("25D9B1A5");

    while(1){
        nfc_init(&context);
        pnd = nfc_open(context, NULL);
        if (pnd == NULL) {
            //qDebug() << "ERROR: %s. Unable to open NFC device.";

        }else{

            qDebug() << "Nfc aperto";

            if (nfc_initiator_init(pnd) < 0) {
                qDebug() << "nfc_initiator_init";
                nfc_close(pnd);
                nfc_exit(context);

            }else{
                qDebug() << "Nfc iniator";
                if(nfc_initiator_select_passive_target(pnd, nmMifare, NULL, 0, &nt) > 0){
                    id = "";
                    vieData = 0;
                    for(i = 0; i < nt.nti.nai.szUidLen;i++){

                        QString hex;
                        id += hex.sprintf("%02x",nt.nti.nai.abtUid[i]).toUpper();

                    }
                    qDebug() << "Leggo: " << id;

                    //wLcd->clear();
                    lcd = "Attendere";
                    lcd = lcd+repeat.repeated(16 - lcd.length());
                    //wLcd->write(0,0,lcd.toUtf8().data());
                    WriteLcdT(0,0,lcd,true);

                    WriteDB(id);
                    /*
                     url = "/default/json/badge/cardkeyw/"+id;
                     resp = http.Get(url);

                     d = QJsonDocument::fromJson(resp.toUtf8(),error);

                     if(error->error==QJsonParseError::NoError){

                         lcd = d.object().value("mess").toString();
                         lcd = lcd+repeat.repeated(16 - lcd.length());
                         //wLcd->write(0,0,lcd.toUtf8().data());
                         WriteLcdT(0,0,lcd,true);

                         lcd = d.object().value("name").toString();
                         lcd = lcd+repeat.repeated(16 - lcd.length());
                         //wLcd->write(0,1,lcd.toUtf8().data());
                         WriteLcdT(0,1,lcd,false);



                     }else{
                         lcd = "Errore Server";
                         lcd = lcd+repeat.repeated(16 - lcd.length());
                         //wLcd->write(0,0,lcd.toUtf8().data());
                         WriteLcdT(0,0,lcd,true);
                     }
                     */
                    while(!nfc_initiator_target_is_present(pnd,&nt)){
                        sleep(1);
                    }
                    sleep(2);
                    //wLcd->clear();
                    lcd = "Attesa badge";
                    lcd = lcd+repeat.repeated(16 - lcd.length());
                    //wLcd->write(0,0,lcd.toUtf8().data());
                    WriteLcdT(0,0,lcd,true);
                    vieData = 1;

                    nfc_close(pnd);
                    nfc_exit(context);

                }
            }
        }
    }

}
void NfcThread::ViewData(){

    if(vieData==1){

        QString txt = QDateTime::currentDateTime().toString("dd-MM   hh:mm:ss")+" ";
        //wLcd->write(0,1,txt.toUtf8().data());
        WriteLcdT(0,1,txt,false);

    }
    //int sec = QTime::currentTime().hour();
    //qDebug() << "Seconds " << sec;
    bool file =QFile::exists("/tmp/forceup");

    if(hour==QTime::currentTime().hour() || file){

        QFile::remove("/tmp/forceup");

        if(writedb==0){

            if(wdbserver->isRunning()){
                qDebug() << "Il thread webdbserver sta ancora girando, controllare";
            }else{
                qDebug() << "Parte thread webdbserver ore " << hour;
                wdbserver->start();
                writedb = 1;
            }
        }

    }else if(writedb){
        qDebug() << "Imposto write db a 0 ";
        writedb = 0;
    }

}

void NfcThread::WriteLcdT(int x,int y, QString data,bool clear){

    if(QString(getenv("USER"))!="alberto"){

        if(clear)
            wLcd->clear();

        wLcd->write(x,y,data.toUpper().toUtf8().data());
    }

}
void NfcThread::WriteDB(QString id){

    QHash<QString,QString> resQ = dao->singleRow("workers","cardkey='"+id+"'");
    field.clear();
    QString lcd,repeat = " ",idW,name,where,autocomplete;
    bool res;
    if(resQ.count()){

        idW = resQ.value("id");
        name = resQ.value("name");
        autocomplete = resQ.value("autocomplete");
        resQ = dao->singleRow("giorni","id_worker='"+idW+"' ORDER BY id DESC LIMIT 1");

        if(resQ.count()==0){

            field.insert("data",QDateTime::currentDateTime().toString("yyyy-MM-dd"));
            field.insert("id_worker",idW);
            field.insert("indt",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

            if(autocomplete=="SI"){

                field.insert("outdt",QDateTime::currentDateTime().addSecs(30540).toString("yyyy-MM-dd hh:mm:ss"));

            }

            res = dao->insertRow("giorni",field);

            if(res){

                lcd = "Entrata";
                lcd = lcd+repeat.repeated(16 - lcd.length());
                //wLcd->write(0,0,lcd.toUtf8().data());
                WriteLcdT(0,0,lcd,true);

                lcd = name;
                lcd = lcd+repeat.repeated(16 - lcd.length());
                //wLcd->write(0,1,lcd.toUtf8().data());
                WriteLcdT(0,1,lcd,false);
            }else{
                lcd = "errore database";
                lcd = lcd+repeat.repeated(16 - lcd.length());
                //wLcd->write(0,0,lcd.toUtf8().data());
                WriteLcdT(0,0,lcd,true);
            }
        }else{

            if(resQ.value("outdt")==""){

                where = "id="+resQ.value("id");

                QStringList sets;
                sets.append("outdt='"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+"'");

                res = dao->updateRow("giorni",sets,where);

                if(res){

                    lcd = "Uscita";
                    lcd = lcd+repeat.repeated(16 - lcd.length());
                    //wLcd->write(0,0,lcd.toUtf8().data());
                    WriteLcdT(0,0,lcd,true);

                    lcd = name;
                    lcd = lcd+repeat.repeated(16 - lcd.length());
                    //wLcd->write(0,1,lcd.toUtf8().data());
                    WriteLcdT(0,1,lcd,false);
                }else{
                    lcd = "errore database";
                    lcd = lcd+repeat.repeated(16 - lcd.length());
                    //wLcd->write(0,0,lcd.toUtf8().data());
                    WriteLcdT(0,0,lcd,true);
                }
            }else{

                if(autocomplete=="NO"){

                    field.insert("data",QDateTime::currentDateTime().toString("yyyy-MM-dd"));
                    field.insert("id_worker",idW);
                    field.insert("indt",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
                    res = dao->insertRow("giorni",field);

                    if(res){

                        lcd = "Entrata";
                        lcd = lcd+repeat.repeated(16 - lcd.length());
                        //wLcd->write(0,0,lcd.toUtf8().data());
                        WriteLcdT(0,0,lcd,true);

                        lcd = name;
                        lcd = lcd+repeat.repeated(16 - lcd.length());
                        //wLcd->write(0,1,lcd.toUtf8().data());
                        WriteLcdT(0,1,lcd,false);
                    }else{
                        lcd = "errore database";
                        lcd = lcd+repeat.repeated(16 - lcd.length());
                        //wLcd->write(0,0,lcd.toUtf8().data());
                        WriteLcdT(0,0,lcd,true);
                    }
                }else{
                    lcd = "GIA' TIMBRATO";
                    lcd = lcd+repeat.repeated(16 - lcd.length());
                    //wLcd->write(0,0,lcd.toUtf8().data());
                    WriteLcdT(0,0,lcd,true);

                    lcd = name;
                    lcd = lcd+repeat.repeated(16 - lcd.length());
                    //wLcd->write(0,1,lcd.toUtf8().data());
                    WriteLcdT(0,1,lcd,false);
                }

            }

        }

    }else{

        HttpClient http;
        QJsonParseError *error = new QJsonParseError();
        QString url = "/default/json/badge/cardkeyw/"+id;
        QString resp = http.Get(url);

        QJsonDocument d = QJsonDocument::fromJson(resp.toUtf8(),error);

        if(error->error==QJsonParseError::NoError){

            lcd = d.object().value("mess").toString();
            lcd = lcd+repeat.repeated(16 - lcd.length());
            //wLcd->write(0,0,lcd.toUtf8().data());
            WriteLcdT(0,0,lcd,true);

            lcd = d.object().value("name").toString();
            lcd = lcd+repeat.repeated(16 - lcd.length());
            //wLcd->write(0,1,lcd.toUtf8().data());
            WriteLcdT(0,1,lcd,false);

        }else{

            lcd = "Errore Server";
            lcd = lcd+repeat.repeated(16 - lcd.length());
            //wLcd->write(0,0,lcd.toUtf8().data());
            WriteLcdT(0,0,lcd,true);
        }
        /*
        lcd = "errore";
        lcd = lcd+repeat.repeated(16 - lcd.length());
        //wLcd->write(0,0,lcd.toUtf8().data());
        WriteLcdT(0,0,lcd,true);

        lcd = "badge non valido";
        lcd = lcd+repeat.repeated(16 - lcd.length());
        //wLcd->write(0,1,lcd.toUtf8().data());
        WriteLcdT(0,1,lcd,false);
        */

    }

}
