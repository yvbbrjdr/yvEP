#ifndef YVEPSOCKET_H
#define YVEPSOCKET_H

#include <QObject>
#include <QVariantMap>
#include <QByteArray>
#include <QJsonDocument>
#include "socketbuffer.h"
#include "socketreceiver.h"
#include "socketsender.h"

class yvEP;

class yvEPSocket : public QObject {
    Q_OBJECT
private:
    QString IP;
    unsigned short Port;
    yvEP *protocol;
public:
    SocketBuffer *buffer;
private:
    SocketSender *sender;
    SocketReceiver *receiver;
public:
    explicit yvEPSocket(const QString &IP,unsigned short Port,yvEP *protocol,QObject *parent = 0);
    void ProcessRaw(const QByteArray &Data);
    void PushData(const QVariantMap &Data);
    ~yvEPSocket();
private slots:
    void SendRaw(const QByteArray &Data);
    void RecvData(const QVariantMap &Data);
    void Reset();
};

#include "yvep.h"

#endif // YVEPSOCKET_H
