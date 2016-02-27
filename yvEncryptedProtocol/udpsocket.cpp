#include "udpsocket.h"

UdpSocket::UdpSocket(unsigned short Port,QObject *parent):QUdpSocket(parent) {
    connect(this,SIGNAL(readyRead()),this,SLOT(ProcessData()));
    connect(this,SIGNAL(SendData(QString,unsigned short,QByteArray)),this,SLOT(SendDataSlot(QString,unsigned short,QByteArray)));
    bind(Port);
}

void UdpSocket::ProcessData() {
    while (hasPendingDatagrams()) {
        QByteArray Data;
        QHostAddress Host;
        unsigned short Port;
        Data.resize(pendingDatagramSize());
        readDatagram(Data.data(),Data.size(),&Host,&Port);
        QString IP=Host.toString();
        if (IP[0]==':')
            IP=IP.mid(7);
        emit RecvData(IP,Port,Data);
    }
}

void UdpSocket::SendDataSlot(const QString &IP,unsigned short Port,const QByteArray &Data) {
    writeDatagram(Data,QHostAddress(IP),Port);
}

UdpSocket::~UdpSocket() {
    close();
}
