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

yvEP::yvEP(unsigned short Port,QObject *parent):QObject(parent),socket(new UdpSocket(Port)),thread(new QThread(this)),LocalPublicKey(crypto_box_PUBLICKEYBYTES,0),LocalPrivateKey(crypto_box_SECRETKEYBYTES,0),KeyPrepared(false),Connecting(false) {
    connect(socket,SIGNAL(RecvData(QString,unsigned short,QByteArray)),this,SLOT(ProcessData(QString,unsigned short,QByteArray)));
    socket->moveToThread(thread);
    thread->start();
}

yvEP::~yvEP() {
    thread->quit();
    thread->wait();
    socket->deleteLater();
}

bool yvEP::Bound() {
    return socket->Bound();
}

void yvEP::GenerateKey() {
    crypto_box_keypair((unsigned char*)LocalPublicKey.data(),(unsigned char*)LocalPrivateKey.data());
    KeyPrepared=true;
}

bool yvEP::LoadKey(const QString &Filename) {
    QFile file(Filename);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    LocalPublicKey=file.read(crypto_box_PUBLICKEYBYTES);
    LocalPrivateKey=file.read(crypto_box_SECRETKEYBYTES);
    file.close();
    return KeyPrepared=true;
}

bool yvEP::SaveKey(const QString &Filename) {
    if (!KeyPrepared)
        return false;
    QFile file(Filename);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    file.write(LocalPublicKey);
    file.write(LocalPrivateKey);
    file.close();
    return true;
}

void yvEP::AutoKey(const QString &Filename) {
    if (!LoadKey(Filename)) {
        GenerateKey();
        SaveKey(Filename);
    }
}

bool yvEP::SendData(const QString &IP,unsigned short Port,const QByteArray &Data) {
    if (!KeyPrepared)
        return false;
    if (crypto_box_MACBYTES+Data.length()+crypto_box_NONCEBYTES>570)
        return false;
    QMap<QPair<QString,unsigned short>,QByteArray>::iterator it=PublicKeys.find(QPair<QString,unsigned short>(IP,Port));
    if (it==PublicKeys.end()) {
        if (Connecting)
            return false;
        emit socket->SendData(IP,Port,'0'+LocalPublicKey);
        QTime t=QTime::currentTime();
        t.start();
        Connecting=true;
        while (t.elapsed()<1000&&(it=PublicKeys.find(QPair<QString,unsigned short>(IP,Port)))==PublicKeys.end())
            QCoreApplication::processEvents();
        it=PublicKeys.find(QPair<QString,unsigned short>(IP,Port));
        Connecting=false;
        if (it==PublicKeys.end())
            return false;
    }
    QByteArray nonce(crypto_box_NONCEBYTES,0),t(crypto_box_MACBYTES+Data.length(),0);
    randombytes_buf(nonce.data(),nonce.length());
    if (crypto_box_easy((unsigned char*)t.data(),(unsigned char*)Data.data(),Data.length(),(unsigned char*)nonce.data(),(unsigned char*)it.value().data(),(unsigned char*)LocalPrivateKey.data()))
        return false;
    emit socket->SendData(IP,Port,'2'+t+nonce);
    return true;
}

bool yvEP::SendAndConfirm(const QString &IP,unsigned short Port,const QByteArray &Data) {
    if (!KeyPrepared)
        return false;
    QMap<QPair<QString,unsigned short>,QByteArray>::iterator it=PublicKeys.find(QPair<QString,unsigned short>(IP,Port));
    if (it==PublicKeys.end()) {
        if (Connecting)
            return false;
        emit socket->SendData(IP,Port,'0'+LocalPublicKey);
        QTime t=QTime::currentTime();
        t.start();
        Connecting=true;
        while (t.elapsed()<1000&&(it=PublicKeys.find(QPair<QString,unsigned short>(IP,Port)))==PublicKeys.end())
            QCoreApplication::processEvents();
        it=PublicKeys.find(QPair<QString,unsigned short>(IP,Port));
        Connecting=false;
        if (it==PublicKeys.end())
            return false;
    }
    QByteArray qba(Data);
    while (crypto_box_MACBYTES+qba.length()+crypto_box_NONCEBYTES>570) {
        QByteArray nonce(crypto_box_NONCEBYTES,0),tmp(crypto_box_MACBYTES+500,0);
        randombytes_buf(nonce.data(),nonce.length());
        if (crypto_box_easy((unsigned char*)tmp.data(),(unsigned char*)qba.data(),500,(unsigned char*)nonce.data(),(unsigned char*)it.value().data(),(unsigned char*)LocalPrivateKey.data())) {
            socket->SendData(IP,Port,"6");
            socket->SendData(IP,Port,"6");
            socket->SendData(IP,Port,"6");
            return false;
        }
        emit socket->SendData(IP,Port,'3'+tmp+nonce);
        QSet<QByteArray>::iterator it2;
        QTime t=QTime::currentTime();
        t.start();
        while (t.elapsed()<1000&&(it2=Accepted.find(nonce))==Accepted.end())
            QCoreApplication::processEvents();
        it2=Accepted.find(nonce);
        if (it2==Accepted.end()) {
            socket->SendData(IP,Port,"6");
            socket->SendData(IP,Port,"6");
            socket->SendData(IP,Port,"6");
            return false;
        }
        Accepted.erase(it2);
        qba=qba.mid(500);
    }
    QByteArray nonce(crypto_box_NONCEBYTES,0),tmp(crypto_box_MACBYTES+qba.length(),0);
    randombytes_buf(nonce.data(),nonce.length());
    if (crypto_box_easy((unsigned char*)tmp.data(),(unsigned char*)qba.data(),qba.length(),(unsigned char*)nonce.data(),(unsigned char*)it.value().data(),(unsigned char*)LocalPrivateKey.data())) {
        socket->SendData(IP,Port,"6");
        socket->SendData(IP,Port,"6");
        socket->SendData(IP,Port,"6");
        return false;
    }
    emit socket->SendData(IP,Port,'4'+tmp+nonce);
    QSet<QByteArray>::iterator it2;
    QTime t=QTime::currentTime();
    t.start();
    while (t.elapsed()<1000&&(it2=Accepted.find(nonce))==Accepted.end())
        QCoreApplication::processEvents();
    it2=Accepted.find(nonce);
    if (it2==Accepted.end()) {
        socket->SendData(IP,Port,"6");
        socket->SendData(IP,Port,"6");
        socket->SendData(IP,Port,"6");
        return false;
    }
    Accepted.erase(it2);
    return true;
}

void yvEP::ProcessData(const QString &IP,unsigned short Port,const QByteArray &Data) {
    if (!KeyPrepared)
        return;
    char op=Data[0];
    QByteArray qba=Data.mid(1);
    if (op=='0') {
        PublicKeys[QPair<QString,unsigned short>(IP,Port)]=qba;
        emit socket->SendData(IP,Port,'1'+LocalPublicKey);
    } else if (op=='1') {
        PublicKeys[QPair<QString,unsigned short>(IP,Port)]=qba;
    } else if (op=='2') {
        QByteArray t(qba.length()-crypto_box_MACBYTES-crypto_box_NONCEBYTES,0);
        QMap<QPair<QString,unsigned short>,QByteArray>::iterator it=PublicKeys.find(QPair<QString,unsigned short>(IP,Port));
        if (it==PublicKeys.end()) {
            emit socket->SendData(IP,Port,'0'+LocalPublicKey);
            return;
        }
        if (crypto_box_open_easy((unsigned char*)t.data(),(unsigned char*)qba.data(),qba.length()-crypto_box_NONCEBYTES,(unsigned char*)qba.data()+qba.length()-crypto_box_NONCEBYTES,(unsigned char*)it.value().data(),(unsigned char*)LocalPrivateKey.data())) {
            emit socket->SendData(IP,Port,'0'+LocalPublicKey);
            return;
        }
        emit RecvData(IP,Port,t);
    } else if (op=='3') {
        QByteArray t(qba.length()-crypto_box_MACBYTES-crypto_box_NONCEBYTES,0);
        QMap<QPair<QString,unsigned short>,QByteArray>::iterator it=PublicKeys.find(QPair<QString,unsigned short>(IP,Port));
        if (it==PublicKeys.end()) {
            emit socket->SendData(IP,Port,'0'+LocalPublicKey);
            return;
        }
        if (crypto_box_open_easy((unsigned char*)t.data(),(unsigned char*)qba.data(),qba.length()-crypto_box_NONCEBYTES,(unsigned char*)qba.data()+qba.length()-crypto_box_NONCEBYTES,(unsigned char*)it.value().data(),(unsigned char*)LocalPrivateKey.data())) {
            emit socket->SendData(IP,Port,'0'+LocalPublicKey);
            return;
        }
        Buffer[QPair<QString,unsigned short>(IP,Port)].append(t);
        emit socket->SendData(IP,Port,'5'+qba.right(crypto_box_NONCEBYTES));
    } else if (op=='4') {
        QByteArray t(qba.length()-crypto_box_MACBYTES-crypto_box_NONCEBYTES,0);
        QMap<QPair<QString,unsigned short>,QByteArray>::iterator it=PublicKeys.find(QPair<QString,unsigned short>(IP,Port));
        if (it==PublicKeys.end()) {
            emit socket->SendData(IP,Port,'0'+LocalPublicKey);
            return;
        }
        if (crypto_box_open_easy((unsigned char*)t.data(),(unsigned char*)qba.data(),qba.length()-crypto_box_NONCEBYTES,(unsigned char*)qba.data()+qba.length()-crypto_box_NONCEBYTES,(unsigned char*)it.value().data(),(unsigned char*)LocalPrivateKey.data())) {
            emit socket->SendData(IP,Port,'0'+LocalPublicKey);
            return;
        }
        Buffer[QPair<QString,unsigned short>(IP,Port)].append(t);
        emit socket->SendData(IP,Port,'5'+qba.right(crypto_box_NONCEBYTES));
        emit RecvData(IP,Port,Buffer[QPair<QString,unsigned short>(IP,Port)]);
        Buffer[QPair<QString,unsigned short>(IP,Port)]="";
    } else if (op=='5') {
        Accepted.insert(qba);
    } else if (op=='6') {
        Buffer[QPair<QString,unsigned short>(IP,Port)]="";
    }
}
