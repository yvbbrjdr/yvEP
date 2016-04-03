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

#include "yvep.h"

yvEP::yvEP(unsigned short Port,QObject *parent):QObject(parent),LocalPublicKey(crypto_box_PUBLICKEYBYTES,0),LocalPrivateKey(crypto_box_SECRETKEYBYTES,0),RemotePublicKey(crypto_box_PUBLICKEYBYTES,0) {
    connecting=false;
    socket=new UdpSocket(Port);
    thread=new QThread(this);
    RemotePort=0;
    crypto_box_keypair((unsigned char*)LocalPublicKey.data(),(unsigned char*)LocalPrivateKey.data());
    connect(socket,SIGNAL(RecvData(QString,unsigned short,QByteArray)),this,SLOT(ProcessData(QString,unsigned short,QByteArray)));
    socket->moveToThread(thread);
    thread->start();
}

bool yvEP::Bound() {
    return socket->Bound();
}

QString yvEP::CurRemoteIP() {
    return RemoteIP;
}

unsigned short yvEP::CurRemotePort() {
    return RemotePort;
}

bool yvEP::ConnectTo(const QString &IP,unsigned short Port) {
    if (RemoteIP==IP&&RemotePort==Port)
        return true;
    if (connecting)
        return false;
    connecting=true;
    emit socket->SendData(IP,Port,"0");
    QTime t=QTime::currentTime();
    t.start();
    while (t.elapsed()<1000&&(RemoteIP!=IP||RemotePort!=Port))
        QCoreApplication::processEvents();
    connecting=false;
    if (RemoteIP==IP&&RemotePort==Port)
        return true;
    return false;
}

bool yvEP::SendData(const QByteArray &Data) {
    if (RemoteIP==""||crypto_box_MACBYTES+Data.length()+crypto_box_NONCEBYTES+crypto_box_PUBLICKEYBYTES>570)
        return false;
    QByteArray nonce(crypto_box_NONCEBYTES,0),t(crypto_box_MACBYTES+Data.length(),0);
    randombytes_buf(nonce.data(),nonce.length());
    if (crypto_box_easy((unsigned char*)t.data(),(unsigned char*)Data.data(),Data.length(),(unsigned char*)nonce.data(),(unsigned char*)RemotePublicKey.data(),(unsigned char*)LocalPrivateKey.data()))
        return false;
    QByteArray qba("2");
    qba.append(t);
    qba.append(nonce);
    qba.append(LocalPublicKey);
    emit socket->SendData(RemoteIP,RemotePort,qba);
    return true;
}

bool yvEP::ConnectAndSend(const QString &IP,unsigned short Port,const QByteArray &Data) {
    if (!ConnectTo(IP,Port))
        return false;
    return SendData(Data);
}

void yvEP::ProcessData(const QString &IP,unsigned short Port,const QByteArray &Data) {
    char op=Data.at(0);
    if (op=='0') {
        QByteArray qba("1");
        qba.append(LocalPublicKey);
        emit socket->SendData(IP,Port,qba);
        emit ConnectYou(IP,Port);
    } else if (op=='1') {
        RemoteIP=IP;
        RemotePort=Port;
        RemotePublicKey=Data.mid(1);
    } else if (op=='2') {
        QByteArray qba=Data.mid(1);
        QByteArray pubkey(qba.right(crypto_box_PUBLICKEYBYTES));
        qba=qba.left(qba.length()-crypto_box_PUBLICKEYBYTES);
        QByteArray nonce(qba.right(crypto_box_NONCEBYTES));
        qba=qba.left(qba.length()-crypto_box_NONCEBYTES);
        QByteArray t(qba.length()-crypto_box_MACBYTES,0);
        if (crypto_box_open_easy((unsigned char*)t.data(),(unsigned char*)qba.data(),qba.length(),(unsigned char*)nonce.data(),(unsigned char*)pubkey.data(),(unsigned char*)LocalPrivateKey.data()))
            return;
        emit RecvData(IP,Port,t);
    }
}

yvEP::~yvEP() {
    thread->quit();
    thread->wait();
    socket->deleteLater();
}
