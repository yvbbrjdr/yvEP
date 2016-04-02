/*
This file is part of yvEncryptedProtocol
yvEncryptedProtocol is an Internet protocol that provides secure connections between computers. 
Copyright (C) 2016  yvbbrjdr

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "udpsocket.h"

UdpSocket::UdpSocket(unsigned short Port,QObject *parent):QUdpSocket(parent),bound(false) {
    connect(this,SIGNAL(readyRead()),this,SLOT(ProcessData()));
    connect(this,SIGNAL(SendData(QString,unsigned short,QByteArray)),this,SLOT(SendDataSlot(QString,unsigned short,QByteArray)));
    bound=bind(Port,QAbstractSocket::DontShareAddress);
}

bool UdpSocket::Bound() {
    return bound;
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
