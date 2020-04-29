#ifndef WRITEDBSERVER_H
#define WRITEDBSERVER_H

#include <QThread>
#include "lib/dao.h"

class WriteDbServer : public QThread
{
    Q_OBJECT
public:
    WriteDbServer(Dao *dObj);
private:
    void run();
    Dao *d;

};

#endif // WRITEDBSERVER_H
