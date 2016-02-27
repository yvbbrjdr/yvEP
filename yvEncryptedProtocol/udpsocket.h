#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <QByteArray>
#include <QUdpSocket>
#include <QHostAddress>

class UdpSocket : public QUdpSocket {
    Q_OBJECT
public:
    explicit UdpSocket(unsigned short Port=0,QObject *parent=0);
    ~UdpSocket();
signals:
    void RecvData(const QString &IP,unsigned short Port,const QByteArray &Data);
    void SendData(const QString &IP,unsigned short Port,const QByteArray &Data);
private slots:
    void ProcessData();
    void SendDataSlot(const QString &IP,unsigned short Port,const QByteArray &Data);
};

#endif // UDPSOCKET_H
