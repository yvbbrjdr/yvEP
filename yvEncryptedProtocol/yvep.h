#ifndef YVEP_H
#define YVEP_H

#include "yvencryptedprotocol_global.h"
#include <QObject>
#include <QByteArray>
#include <openssl/rsa.h>
#include <QString>
#include <QThread>
#include "udpsocket.h"

class YVENCRYPTEDPROTOCOLSHARED_EXPORT yvEP : public QObject {
    Q_OBJECT
public:
    explicit yvEP(unsigned short Port=0,QObject *parent=0);
    ~yvEP();
    QString CurRemoteIP();
    unsigned short CurRemotePort();
private:
    UdpSocket *socket;
    QThread *thread;
    RSA *LocalRSA,*RemoteRSA;
    QString RemoteIP;
    unsigned short RemotePort;
    QByteArray PublicKey;
signals:
    void RecvData(const QString &IP,unsigned short Port,const QByteArray &Data);
    void ConnectYou(const QString &IP,unsigned short Port);
    void ConnectedTo(const QString &IP,unsigned short Port);
public slots:
    void ConnectTo(const QString &IP,unsigned short Port);
    void SendData(const QByteArray &Data);
private slots:
    void ProcessData(const QString &IP,unsigned short Port,const QByteArray &Data);
};

#endif // YVEP_H
