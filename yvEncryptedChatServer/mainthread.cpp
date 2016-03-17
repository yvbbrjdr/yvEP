/*
This file is part of yvEncryptedChatServer
yvEncryptedChatServer is an IM program server that provides secure chats. 
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

#include "mainthread.h"

MainThread::MainThread(unsigned short Port) {
    server=new Server(Port);
    connect(this,SIGNAL(RemoveClient(QString)),server,SLOT(RemoveClient(QString)));
}

void MainThread::run() {
    while (1) {
        char tmp[20];
        QString n(fgets(tmp,17,stdin));
        n=n.left(n.length()-1);
        emit RemoveClient(n);
    }
}