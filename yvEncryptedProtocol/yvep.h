#ifndef YVEP_H
#define YVEP_H

#include "yvencryptedprotocol_global.h"
#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <openssl/rsa.h>
#include <QString>

class YVENCRYPTEDPROTOCOLSHARED_EXPORT yvEP : public QObject {
    Q_OBJECT
public:
    yvEP(unsigned short Port,QObject *parent=0);
    void Send(const QString &IP,unsigned short Port,const QByteArray &Data);
    ~yvEP();
private:
    QUdpSocket *socket;
signals:
    void Recv(const QString &IP,unsigned short Port,const QByteArray &Data);
private slots:
    void ProcessData();
};

#endif // YVEP_H
