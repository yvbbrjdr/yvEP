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

#include <QCoreApplication>
#include "mainthread.h"
#include <QString>
#include <cstdio>
#include <cstdlib>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    if (argc>2) {
        printf("Usage: %s [Port]\n",argv[0]);
        exit(0);
    }
    unsigned short Port=2428;
    if (argc==2)
        Port=QString(argv[1]).toShort();
    puts("Welcome to yvEncryptedChatServer\n");
    fflush(stdout);
    MainThread thread(Port);
    if (!thread.server->protocol->Bound()) {
        printf("Failed to bind to Port %d, exiting. . . \n",Port);
        exit(0);
    }
    thread.start();
    printf("The server has bind to Port %d\nEnter nickname to logout\n",Port);
    return a.exec();
}
