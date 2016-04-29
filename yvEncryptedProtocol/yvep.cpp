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

yvEP::yvEP(QObject *parent):QObject(parent),socket(new UdpSocket),socketthread(new QThread(this)),PublicKey(QByteArray(crypto_box_PUBLICKEYBYTES,0)),PrivateKey(QByteArray(crypto_box_SECRETKEYBYTES,0)) {
    if (sodium_init()==-1)
        exit(-1);
    crypto_box_keypair((unsigned char*)PublicKey.data(),(unsigned char*)PrivateKey.data());
    connect(socket,SIGNAL(RecvData(QString,unsigned short,QByteArray)),this,SLOT(ProcessRaw(QString,unsigned short,QByteArray)));
}

bool yvEP::Bind(unsigned int Port) {
    bool r=socket->bind(Port,QAbstractSocket::DontShareAddress);
    socket->moveToThread(socketthread);
    socketthread->start();
    return r;
}

bool yvEP::ConnectTo(const QString &IP,unsigned short Port) {
    if (Remotes.find(Addr(IP,Port))!=Remotes.end())
        return false;
    QMap<Addr,yvEPSocket*>::iterator it=Remotes.insert(Addr(IP,Port),new yvEPSocket(IP,Port,this,this));
    it.value()->buffer->mutex.lock();
    it.value()->buffer->SendNonce.resize(crypto_box_NONCEBYTES);
    randombytes_buf(it.value()->buffer->SendNonce.data(),it.value()->buffer->SendNonce.length());
    it.value()->buffer->ReplyNonce=it.value()->buffer->SendNonce;
    QByteArray qba(char(StatusFlag::Connect)+PublicKey+it.value()->buffer->SendNonce);
    SendRaw(IP,Port,qba);
    it.value()->buffer->mutex.unlock();
    return true;
}

bool yvEP::SendData(const QString &IP,unsigned short Port,const QVariantMap &Data) {
    QMap<Addr,yvEPSocket*>::iterator it=Remotes.find(Addr(IP,Port));
    if (it==Remotes.end())
        return false;
    it.value()->PushData(Data);
    return true;
}

void yvEP::SendRaw(const QString &IP,unsigned short Port,const QByteArray &Data) {
    emit socket->SendData(IP,Port,Data);
}

void yvEP::ProcessRaw(const QString &IP,unsigned short Port,const QByteArray &Data) {
    QMap<Addr,yvEPSocket*>::iterator it=Remotes.find(Addr(IP,Port));
    if (Data.at(0)&StatusFlag::Connect) {
        if (Data.at(0)&StatusFlag::Acknowledge) {
            if (it==Remotes.end()) {
                ResetRemote(IP,Port);
            } else {
                it.value()->buffer->mutex.lock();
                it.value()->buffer->PublicKey=Data.mid(1,crypto_box_PUBLICKEYBYTES);
                it.value()->buffer->RecvNonce=Data.right(crypto_box_NONCEBYTES);
                it.value()->buffer->mutex.unlock();
                emit Connected(IP,Port);
            }
        } else {
            if (it!=Remotes.end()) {
                ResetRemote(IP,Port);
            } else {
                QMap<Addr,yvEPSocket*>::iterator it=Remotes.insert(Addr(IP,Port),new yvEPSocket(IP,Port,this,this));
                it.value()->buffer->mutex.lock();
                it.value()->buffer->SendNonce.resize(crypto_box_NONCEBYTES);
                randombytes_buf(it.value()->buffer->SendNonce.data(),it.value()->buffer->SendNonce.length());
                it.value()->buffer->ReplyNonce=it.value()->buffer->SendNonce;
                QByteArray qba(char(StatusFlag::Connect|StatusFlag::Acknowledge)+PublicKey+it.value()->buffer->SendNonce);
                SendRaw(IP,Port,qba);
                it.value()->buffer->PublicKey=Data.mid(1,crypto_box_PUBLICKEYBYTES);
                it.value()->buffer->RecvNonce=Data.right(crypto_box_NONCEBYTES);
                it.value()->buffer->ReplyNonce=it.value()->buffer->SendNonce;
                it.value()->buffer->mutex.unlock();
            }
        }
    } else if (Data.at(0)&StatusFlag::Data) {
        if (it==Remotes.end()) {
            ResetRemote(IP,Port);
        } else {
            it.value()->ProcessRaw(Data);
        }
    } else if (Data.at(0)&StatusFlag::Reset) {
        if (it!=Remotes.end()) {
            ResetRemote(IP,Port);
        }
    }
}

void yvEP::ResetRemote(const QString &IP,unsigned short Port) {
    SendRaw(IP,Port,QByteArray(1,char(StatusFlag::Reset)));
    QMap<Addr,yvEPSocket*>::iterator it=Remotes.find(Addr(IP,Port));
    if (it!=Remotes.end()) {
        it.value()->deleteLater();
        Remotes.erase(it);
        emit Reset(IP,Port);
    }
}

unsigned long yvEP::BufferRemain() {
    unsigned long ret=0;
    for (QMap<Addr,yvEPSocket*>::iterator it=Remotes.begin();it!=Remotes.end();++it) {
        it.value()->buffer->mutex.lock();
        ret+=it.value()->buffer->SendBuf.size();
        ret+=it.value()->buffer->RecvBuf.size();
        it.value()->buffer->mutex.unlock();
    }
    return ret;
}

yvEP::~yvEP() {
    socketthread->quit();
    socketthread->wait();
    delete socket;
}
