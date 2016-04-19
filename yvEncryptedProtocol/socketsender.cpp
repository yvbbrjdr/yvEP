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

#include "socketsender.h"

SocketSender::SocketSender(SocketBuffer *buf,QObject *parent):QThread(parent),buf(buf),stop(false),count(0),count1(0),count2(0) {}

void SocketSender::run() {
    while (!stop) {
        buf->mutex.lock();
        if (buf->PublicKey.size()==0) {
            ++count;
            if (count==100000)
                emit Reset();
            buf->mutex.unlock();
            msleep(1);
            continue;
        }
        if (buf->SendNonce!=buf->ReplyNonce) {
            ++count1;
            if (count1==10000) {
                ++count2;
                if (count2==10) {
                    buf->mutex.unlock();
                    emit Reset();
                    break;
                }
                count1=0;
                emit SendRaw(buf->LastSend);
            }
            buf->mutex.unlock();
            msleep(1);
            continue;
        }
        if (buf->SendBuf.size()==0) {
            buf->mutex.unlock();
            msleep(1);
            continue;
        }
        count1=count2=0;
        sodium_increment((unsigned char*)buf->SendNonce.data(),buf->SendNonce.length());
        buf->LastSend=buf->SendBuf.left(512);
        buf->SendBuf=buf->SendBuf.mid(512);
        emit SendRaw(buf->LastSend);
        buf->mutex.unlock();
    }
}

void SocketSender::Stop() {
    stop=true;
}
