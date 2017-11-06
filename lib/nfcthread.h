#ifndef NFCTHREAD_H
#define NFCTHREAD_H


#include <QObject>
#include <nfc/nfc.h>
#include <QThread>

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
};

#endif // NFCTHREAD_H
