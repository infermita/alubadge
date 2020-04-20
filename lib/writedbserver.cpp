#include "writedbserver.h"
#include <QDebug>

WriteDbServer::WriteDbServer()
{

}
void WriteDbServer::run(){

    qDebug() << "Write db on server";
    sleep(62);
    qDebug() << "Finito Write db on server";

}
