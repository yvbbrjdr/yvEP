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
    protocol->AutoKey(QCoreApplication::applicationDirPath()+"/yvECS.key");
    connect(protocol,SIGNAL(RecvData(QString,unsigned short,QByteArray)),this,SLOT(RecvData(QString,unsigned short,QByteArray)));
}

void Server::Log(const QString &Content) {
    fprintf(stderr,"[%s] %s\n",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toUtf8().data(),Content.toUtf8().data());
}

void Server::RecvData(const QString &IP,unsigned short Port,const QByteArray &Data) {
    if (Data.left(2)=="l0") {
        if (Clients.find(Data.mid(2))==Clients.end()&&Data.mid(2)!="Broadcast") {
            Log(QString("%1:%2 logged in as nickname %3").arg(IP).arg(Port).arg(QString(Data.mid(2))));
            Clients.insert(Data.mid(2),UserData(IP,Port));
            protocol->SendData(IP,Port,"l1");
            for (QMap<QString,UserData>::iterator it=Clients.begin();it!=Clients.end();++it)
                protocol->SendData(it.value().IP,it.value().Port,"t3");
        } else {
            protocol->SendData(IP,Port,"l2");
            Log(QString("%1:%2 tried to log in as nickname %3 but failed").arg(IP).arg(Port).arg(QString(Data.mid(2))));
        }
    } else if (Data.left(2)=="li") {
        QString s("li\n");
        for (QMap<QString,UserData>::iterator it=Clients.begin();it!=Clients.end();++it) {
            if (it.value().Cloak)
                continue;
            s+=it.key()+'\n';
        }
        if (protocol->SendAndConfirm(IP,Port,s.left(s.length()-1).toUtf8()))
            Log(QString("%1:%2 requested the client list").arg(IP).arg(Port));
        else
            Log(QString("%1:%2 requested the client list but failed").arg(IP).arg(Port));
    } else if (Data.left(2)=="l3") {
        QString n(Data.mid(2));
        QMap<QString,UserData>::iterator it=Clients.find(n);
        if (it!=Clients.end()&&it.value().IP==IP&&it.value().Port==Port) {
            Clients.erase(it);
            Log(QString("%1(%2:%3) logged off").arg(QString(Data.mid(2))).arg(IP).arg(Port));
            for (QMap<QString,UserData>::iterator it=Clients.begin();it!=Clients.end();++it)
                protocol->SendData(it.value().IP,it.value().Port,"t3");
        }
    } else if (Data.left(2)=="t0") {
        if (Clients.find(Data.mid(2))==Clients.end()||Clients.find(Data.mid(2)).value().Cloak) {
            protocol->SendData(IP,Port,"t3");
            Log(QString("%1:%2 requested the address of %3 but failed").arg(IP).arg(Port).arg(QString(Data.mid(2))));
        } else {
            UserData ipp=Clients.find(Data.mid(2)).value();
            protocol->SendData(IP,Port,("t1"+ipp.IP+':'+QString::number(ipp.Port)+':'+Data.mid(2)).toUtf8());
            protocol->SendData(ipp.IP,ipp.Port,("t2"+IP+':'+QString::number(Port)).toUtf8());
            Log(QString("%1:%2 requested the address of %3(%4:%5)").arg(IP).arg(Port).arg(QString(Data.mid(2))).arg(ipp.IP).arg(ipp.Port));
        }
    } else if (Data[0]=='f') {
        QString n(Data.mid(1,Data.indexOf('\n')-1));
        QMap<QString,UserData>::iterator it=Clients.find(n);
        if (it!=Clients.end()) {
            protocol->SendAndConfirm(it.value().IP,it.value().Port,'m'+Data.mid(Data.indexOf('\n')+1));
        }
    } else if (Data[0]=='b') {
        for (QMap<QString,UserData>::iterator it=Clients.begin();it!=Clients.end();++it)
            if (it.value().IP!=IP||it.value().Port!=Port)
                protocol->SendData(it.value().IP,it.value().Port,Data);
    } else if (Data.left(2)=="c0") {
        QString n(Data.mid(2));
        QMap<QString,UserData>::iterator it=Clients.find(n);
        if (it!=Clients.end()&&it.value().IP==IP&&it.value().Port==Port) {
            it.value().Cloak=!it.value().Cloak;
            if (it.value().Cloak) {
                protocol->SendData(IP,Port,"c1");
                Log(QString("%1(%2:%3) cloaked").arg(QString(Data.mid(2))).arg(IP).arg(Port));
            } else {
                protocol->SendData(IP,Port,"c2");
                Log(QString("%1(%2:%3) decloaked").arg(QString(Data.mid(2))).arg(IP).arg(Port));
            }
            for (QMap<QString,UserData>::iterator it=Clients.begin();it!=Clients.end();++it)
                protocol->SendData(it.value().IP,it.value().Port,"t3");
        }
    }
}

void Server::RemoveClient(const QString &Nickname) {
    QMap<QString,UserData>::iterator it=Clients.find(Nickname);
    if (it!=Clients.end()) {
        Log(QString("%1(%2:%3) is forced to log off").arg(Nickname).arg(it.value().IP).arg(it.value().Port));
        protocol->SendData(it.value().IP,it.value().Port,"l3");
        Clients.remove(Nickname);
        for (QMap<QString,UserData>::iterator it=Clients.begin();it!=Clients.end();++it)
            protocol->SendData(it.value().IP,it.value().Port,"t3");
    } else {
        Log(QString("%1 is not found").arg(Nickname));
    }
}
