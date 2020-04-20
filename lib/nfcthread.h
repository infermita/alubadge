#ifndef NFCTHREAD_H
#define NFCTHREAD_H


#include <QObject>
#include <nfc/nfc.h>
#include <QThread>
#include <QTimer>
#include "lib/writelcd.h"
#include "lib/writedbserver.h"

class NfcThread : public QThread
{
    Q_OBJECT
public:
    NfcThread();
private:
    void run();
    nfc_target nt;
    nfc_context *context;
    nfc_device *pnd;
    int vieData;
    WriteLcd *wLcd;
    int writedb;
    QTimer *tdb;
    void WriteLcdT(int x,int y, QString data,bool clear);
    WriteDbServer wdbserver;
public slots:
    void ViewData();
};

#endif // NFCTHREAD_H
