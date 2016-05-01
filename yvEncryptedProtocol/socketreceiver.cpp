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

#include "socketreceiver.h"

SocketReceiver::SocketReceiver(SocketBuffer *buf,QObject *parent):QThread(parent),buf(buf),stop(false) {}

void SocketReceiver::run() {
    while (!stop) {
        buf->mutex.lock();
        int e=buf->RecvBuf.indexOf(char(23));
        if (e==-1) {
            buf->mutex.unlock();
            msleep(1);
            continue;
        }
        emit RecvData(QJsonDocument::fromJson(buf->RecvBuf.left(e)).toVariant().toMap());
        buf->RecvBuf=buf->RecvBuf.mid(e+1);
        buf->mutex.unlock();
    }
}

void SocketReceiver::Stop() {
    stop=true;
}
