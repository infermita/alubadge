#include "nfcthread.h"
#include "lib/httpclient.h"
#include <QDebug>
#include <QNetworkInterface>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include <QDateTime>

NfcThread::NfcThread()
{

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
    wLcd = new WriteLcd();
    QJsonParseError *error = new QJsonParseError();
    QJsonDocument d;
    vieData = 1;

    wLcd->clear();
    wLcd->write(0,0,"Attesa rete");

    while(ipcheck){

        ip = QNetworkInterface::interfaceFromName("wlan0").addressEntries().first().ip().toString();

        qDebug() << "Attesa rete con ip: " << ip << " count: " << QString::number(ip.split(".").count());

        if(ip.split(".").count()==4)
            ipcheck = false;
        else
            sleep(1);
    }


    wLcd->clear();
    wLcd->write(0,0,"Attesa badge");

    QTimer *viewDet;
    viewDet = new QTimer();
    connect(viewDet, SIGNAL(timeout()),
              this, SLOT(ViewData()),Qt::DirectConnection);
    viewDet->start(1000);

    while(1){
         nfc_init(&context);
         pnd = nfc_open(context, NULL);
         if (pnd == NULL) {
             qDebug() << "ERROR: %s. Unable to open NFC device.";

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
                     wLcd->write(0,0,"Attesa badge");
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
        wLcd->write(0,1,txt.toUtf8().data());

    }

}
