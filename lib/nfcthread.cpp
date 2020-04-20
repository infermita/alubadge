#include "nfcthread.h"
#include "lib/httpclient.h"
#include <QDebug>
#include <QNetworkInterface>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include <QDateTime>
#include <QJsonArray>
#include "dao.h"

NfcThread::NfcThread()
{

    QTimer *viewDet;
    viewDet = new QTimer();
    connect(viewDet, SIGNAL(timeout()),
              this, SLOT(ViewData()),Qt::DirectConnection);
    viewDet->start(1000);
    writedb = 0;

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
    Dao dao;

    //wLcd->clear();
    //wLcd->write(0,0,"Attesa rete     ");
    WriteLcdT(0,0,"Attesa rete     ",true);


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

        dao.deleteRow("cards","1");

        QJsonObject jObj = d.object();
        QJsonArray listUsers;
        QHash<QString,QString> field;

        listUsers = jObj["list"].toArray();
        foreach (const QJsonValue & users, listUsers) {

            field.clear();

            QJsonObject o = users.toObject();
            qDebug() << "Id user: " << o["id"].toString();
            field.insert("id",o["id"].toString());
            field.insert("cardkey",o["cardkey"].toString());
            field.insert("`name`",o["name_lastname"].toString());

            if(dao.replaceRow("workers",field)){
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

                     wLcd->clear();
                     lcd = "Attendere";
                     lcd = lcd+repeat.repeated(16 - lcd.length());
                     wLcd->write(0,0,lcd.toUtf8().data());

                     url = "/default/json/badge/cardkeyw/"+id;
                     resp = http.Get(url);

                     d = QJsonDocument::fromJson(resp.toUtf8(),error);

                     if(error->error==QJsonParseError::NoError){

                         lcd = d.object().value("mess").toString();
                         lcd = lcd+repeat.repeated(16 - lcd.length());
                         wLcd->write(0,0,lcd.toUtf8().data());

                         lcd = d.object().value("name").toString();
                         lcd = lcd+repeat.repeated(16 - lcd.length());
                         wLcd->write(0,1,lcd.toUtf8().data());



                     }else{
                         lcd = "Errore Server";
                         lcd = lcd+repeat.repeated(16 - lcd.length());
                         wLcd->write(0,0,lcd.toUtf8().data());
                     }

                     while(!nfc_initiator_target_is_present(pnd,&nt)){
                         sleep(1);
                     }
                     sleep(2);
                     wLcd->clear();
                     lcd = "Attesa badge";
                     lcd = lcd+repeat.repeated(16 - lcd.length());
                     wLcd->write(0,0,lcd.toUtf8().data());
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
    int sec = QTime::currentTime().second();
    //qDebug() << "Seconds " << sec;
    if(sec==0){

        if(!wdbserver.isRunning())
            wdbserver.start();

    }

}

void NfcThread::WriteLcdT(int x,int y, QString data,bool clear){

    if(QString(getenv("USER"))!="alberto"){

        if(clear)
            wLcd->clear();

        wLcd->write(x,y,data.toUtf8().data());
    }

}
