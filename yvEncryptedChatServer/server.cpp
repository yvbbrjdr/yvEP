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

#include "server.h"

Server::Server(unsigned short Port) {
    protocol=new yvEP(Port);
    connect(protocol,SIGNAL(RecvData(QString,unsigned short,QByteArray)),this,SLOT(RecvData(QString,unsigned short,QByteArray)));
}

void Server::RecvData(const QString &IP,unsigned short Port,const QByteArray &Data) {
    if (Data.left(2)=="l0") {
        if (Clients.find(Data.mid(2))==Clients.end()) {
            Clients.insert(Data.mid(2),UserData(IP,Port));
            protocol->ConnectAndSend(IP,Port,"l1");
        } else
            protocol->ConnectAndSend(IP,Port,"l2");
    } else if (Data.left(2)=="li") {
        QString s("li\n");
        int count=0;
        for (QMap<QString,UserData>::iterator it=Clients.begin();it!=Clients.end();++it) {
            if (it.value().Cloak)
                continue;
            s+=it.key()+'\n';
            ++count;
            if (count%20==0) {
                protocol->ConnectAndSend(IP,Port,s.left(s.length()-1).toUtf8());
                s="li\n";
            }
        }
        protocol->ConnectAndSend(IP,Port,s.left(s.length()-1).toUtf8());
    } else if (Data.left(2)=="l3") {
        QString n(Data.mid(2));
        QMap<QString,UserData>::iterator it=Clients.find(n);
        if (it!=Clients.end()&&it.value().IP==IP&&it.value().Port==Port) {
            Clients.erase(it);
        }
    } else if (Data.left(2)=="l4") {
        QString n(Data.mid(2));
        QMap<QString,UserData>::iterator it=Clients.find(n);
        if (it!=Clients.end()&&it.value().IP==IP&&it.value().Port==Port) {
            it.value().Cloak=!it.value().Cloak;
        }
    } else if (Data.left(2)=="t0") {
        if (Clients.find(Data.mid(2))==Clients.end()||Clients.find(Data.mid(2)).value().Cloak)
            protocol->ConnectAndSend(IP,Port,"t3");
        else {
            UserData ipp=Clients.find(Data.mid(2)).value();
            protocol->ConnectAndSend(IP,Port,("t1"+ipp.IP+':'+QString::number(ipp.Port)+':'+Data.mid(2)).toUtf8());
            protocol->ConnectAndSend(ipp.IP,ipp.Port,("t2"+IP+':'+QString::number(Port)).toUtf8());
        }
    }
}

void Server::RemoveClient(const QString &Nickname) {
    Clients.remove(Nickname);
}
