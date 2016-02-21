#include "yvep.h"

yvEP::yvEP(unsigned short Port,QObject *parent):QObject(parent) {
    socket=new QUdpSocket(this);
    connect(socket,SIGNAL(readyRead()),this,SLOT(ProcessData()));
    socket->bind(Port);
}

void yvEP::Send(const QString &IP,unsigned short Port,const QByteArray &Data) {
    socket->writeDatagram(Data,QHostAddress(IP),Port);
}

yvEP::~yvEP() {
    socket->close();
}

void yvEP::ProcessData() {
    while (socket->hasPendingDatagrams()) {
        QByteArray Data;
        QHostAddress Host;
        unsigned short Port;
        Data.resize(socket->pendingDatagramSize());
        socket->readDatagram(Data.data(),Data.size(),&Host,&Port);
        QString IP=Host.toString();
        if (IP[0]==':')
            IP=IP.mid(7);
        emit Recv(IP,Port,Data);
    }
}
