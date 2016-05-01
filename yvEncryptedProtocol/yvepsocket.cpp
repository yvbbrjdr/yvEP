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

#include "yvepsocket.h"

yvEPSocket::yvEPSocket(const QString &IP,unsigned short Port,yvEP *protocol,QObject *parent):QObject(parent),IP(IP),Port(Port),protocol(protocol) {
    buffer=new SocketBuffer();
    sender=new SocketSender(buffer,this);
    connect(sender,SIGNAL(Reset()),this,SLOT(Reset()));
    connect(sender,SIGNAL(SendRaw(QByteArray)),this,SLOT(SendRaw(QByteArray)));
    receiver=new SocketReceiver(buffer,this);
    connect(receiver,SIGNAL(RecvData(QVariantMap)),this,SLOT(RecvData(QVariantMap)));
    sender->start();
    receiver->start();
}

void yvEPSocket::SendRaw(const QByteArray &Data) {
    buffer->mutex.lock();
    QByteArray t(crypto_box_MACBYTES+Data.length(),0);
    if (crypto_box_easy((unsigned char*)t.data(),(unsigned char*)Data.data(),Data.length(),(unsigned char*)buffer->SendNonce.data(),(unsigned char*)buffer->PublicKey.data(),(unsigned char*)protocol->PrivateKey.data()))
        Reset();
    protocol->SendRaw(IP,Port,char(StatusFlag::Data)+t+buffer->SendNonce);
    buffer->mutex.unlock();
}

void yvEPSocket::ProcessRaw(const QByteArray &Data) {
    buffer->mutex.lock();
    if (Data.at(0)&StatusFlag::Acknowledge) {
        if (buffer->SendNonce==Data.mid(1))
            buffer->ReplyNonce=buffer->SendNonce;
    } else {
        sodium_increment((unsigned char*)buffer->RecvNonce.data(),buffer->RecvNonce.size());
        if (buffer->RecvNonce==Data.right(crypto_box_NONCEBYTES)) {
            QByteArray t(Data.length()-crypto_box_MACBYTES-crypto_box_NONCEBYTES-1,0);
            if (crypto_box_open_easy((unsigned char*)t.data(),(unsigned char*)Data.data()+1,Data.length()-crypto_box_NONCEBYTES-1,(unsigned char*)Data.right(crypto_box_NONCEBYTES).data(),(unsigned char*)buffer->PublicKey.data(),(unsigned char*)protocol->PrivateKey.data()))
                Reset();
            buffer->RecvBuf.append(t);
        } else {
            buffer->RecvNonce=Data.right(crypto_box_NONCEBYTES);
        }
        protocol->SendRaw(IP,Port,char(StatusFlag::Data|StatusFlag::Acknowledge)+buffer->RecvNonce);
    }
    buffer->mutex.unlock();
}

void yvEPSocket::PushData(const QVariantMap &Data) {
    buffer->mutex.lock();
    buffer->SendBuf.append(QJsonDocument::fromVariant(Data).toJson()+char(23));
    buffer->mutex.unlock();
}

void yvEPSocket::RecvData(const QVariantMap &Data) {
    emit protocol->RecvData(IP,Port,Data);
}

void yvEPSocket::Reset() {
    protocol->ResetRemote(IP,Port);
}

yvEPSocket::~yvEPSocket() {
    sender->Stop();
    sender->wait();
    receiver->Stop();
    receiver->wait();
    delete buffer;
}
