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

SocketSender::SocketSender(SocketBuffer *buf,QObject *parent):QThread(parent),buf(buf),stop(false),count(0) {
    time=QTime::currentTime();
}

void SocketSender::run() {
    while (!stop) {
        buf->mutex.lock();
        if (buf->PublicKey.size()==0) {
            ++count;
            if (time.secsTo(QTime::currentTime())>=10)
                emit Reset();
            buf->mutex.unlock();
            msleep(1);
            continue;
        }
        if (buf->SendNonce!=buf->ReplyNonce) {
            if (time.msecsTo(QTime::currentTime())>=1000) {
                ++count;
                if (count==10) {
                    buf->mutex.unlock();
                    emit Reset();
                    break;
                }
                time=QTime::currentTime();
                emit SendRaw(buf->LastSend);
            }
            buf->mutex.unlock();
            msleep(0);
            continue;
        }
        if (buf->SendBuf.size()==0) {
            buf->mutex.unlock();
            time=QTime::currentTime();
            msleep(1);
            continue;
        }
        count=0;
        sodium_increment((unsigned char*)buf->SendNonce.data(),buf->SendNonce.length());
        buf->LastSend=buf->SendBuf.left(1400);
        buf->SendBuf=buf->SendBuf.mid(1400);
        emit SendRaw(buf->LastSend);
        buf->mutex.unlock();
        time=QTime::currentTime();
    }
}

void SocketSender::Stop() {
    stop=true;
}
