#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QString>
#include <QFileInfoList>
#include <QDir>
#include "lib/nfcthread.h"

void myMessageHandler(QtMsgType type, const QMessageLogContext &context,const QString &msg)
{
    QString txt = QDateTime::currentDateTime().toString("dd-MM-yy hh:mm:ss")+" ";
    switch (type) {
    case QtDebugMsg:
        txt += "Debug: " + msg;
        break;
    case QtWarningMsg:
        txt += "Warning: " + msg;
    break;
    case QtCriticalMsg:
        txt += "Critical: " + msg;
    break;
    case QtFatalMsg:
        txt += "Fatal: " + msg;
        abort();
    }
    QFile outFile;
    QString pathLog = "/var/log/";
    if(QString(getenv("USER"))=="alberto"){

        pathLog = "";

    }

    outFile.setFileName(pathLog+"alubadge.log");

    outFile.open(QIODevice::WriteOnly | QIODevice::Append);

    if(outFile.size()>15728640){

        QDir dir(pathLog);
        QStringList filter;
        filter << "alucount.*.log";
        dir.setNameFilters(filter);

        QFileInfoList listFs = dir.entryInfoList(QDir::Files,QDir::Time);

        if(listFs.size()>5){
            for(int i = 5; i < listFs.size();i++){

                QFile::remove(listFs.at(i).absoluteFilePath());

            }
        }

        outFile.rename(pathLog+"alucount.log",pathLog+"alucount."+QDateTime::currentDateTime().toString("dd-MM-yy_hh:mm")+".log");
        outFile.close();
        outFile.setFileName(pathLog+"alucount.log");
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);

    }


    QTextStream ts(&outFile);
    ts << txt << endl;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

     qInstallMessageHandler(myMessageHandler);

    NfcThread nfcTh;
    nfcTh.start();

    return a.exec();
}
