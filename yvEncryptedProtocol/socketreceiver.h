#ifndef SOCKETRECEIVER_H
#define SOCKETRECEIVER_H

#include <QThread>
#include <QJsonDocument>
#include <QVariantMap>
#include "socketbuffer.h"

class SocketReceiver : public QThread {
    Q_OBJECT
private:
    SocketBuffer *buf;
    bool stop;
public:
    explicit SocketReceiver(SocketBuffer *buf,QObject *parent = 0);
    void Stop();
signals:
    void RecvData(const QVariantMap &Data);
protected:
    void run();
};

#endif // SOCKETRECEIVER_H
