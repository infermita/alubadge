#ifndef WRITEDBSERVER_H
#define WRITEDBSERVER_H

#include <QThread>

class WriteDbServer : public QThread
{
    Q_OBJECT
public:
    WriteDbServer();
private:
    void run();
};

#endif // WRITEDBSERVER_H
