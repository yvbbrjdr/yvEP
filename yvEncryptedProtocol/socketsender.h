#ifndef SOCKETSENDER_H
#define SOCKETSENDER_H

#include <QThread>
#include <sodium.h>
#include "socketbuffer.h"

class SocketSender : public QThread {
    Q_OBJECT
private:
    SocketBuffer *buf;
    bool stop;
    int count1,count2;
public:
    explicit SocketSender(SocketBuffer *buf,QObject *parent=0);
    void Stop();
signals:
    void SendRaw(const QByteArray &Data);
    void Reset();
protected:
    void run();
};

#endif // SOCKETSENDER_H
