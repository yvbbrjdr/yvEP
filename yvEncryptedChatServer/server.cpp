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

void Server::Log(const QString &Content) {
    fprintf(stderr,"[%s] %s\n",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toUtf8().data(),Content.toUtf8().data());
}

void Server::RecvData(const QString &IP,unsigned short Port,const QByteArray &Data) {
    if (Data.left(2)=="l0") {
        if (Clients.find(Data.mid(2))==Clients.end()&&Data.mid(2)!="Broadcast") {
            Clients.insert(Data.mid(2),UserData(IP,Port));
            protocol->ConnectAndSend(IP,Port,"l1");
            Log(QString("%1:%2 logged in as nickname %3").arg(IP).arg(Port).arg(QString(Data.mid(2))));
        } else {
            protocol->ConnectAndSend(IP,Port,"l2");
            Log(QString("%1:%2 tried to log in as nickname %3 but failed").arg(IP).arg(Port).arg(QString(Data.mid(2))));
        }
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
        Log(QString("%1:%2 requested the client list").arg(IP).arg(Port));
    } else if (Data.left(2)=="l3") {
        QString n(Data.mid(2));
        QMap<QString,UserData>::iterator it=Clients.find(n);
        if (it!=Clients.end()&&it.value().IP==IP&&it.value().Port==Port) {
            Clients.erase(it);
            Log(QString("%1(%2:%3) logged off").arg(QString(Data.mid(2))).arg(IP).arg(Port));
        } else {
            Log(QString("%1:%2 faked the log off of %3 but failed").arg(IP).arg(Port).arg(QString(Data.mid(2))));
        }
    } else if (Data.left(2)=="t0") {
        if (Clients.find(Data.mid(2))==Clients.end()||Clients.find(Data.mid(2)).value().Cloak) {
            protocol->ConnectAndSend(IP,Port,"t3");
            Log(QString("%1:%2 requested the address of %3 but failed").arg(IP).arg(Port).arg(QString(Data.mid(2))));
        } else {
            UserData ipp=Clients.find(Data.mid(2)).value();
            protocol->ConnectAndSend(IP,Port,("t1"+ipp.IP+':'+QString::number(ipp.Port)+':'+Data.mid(2)).toUtf8());
            protocol->ConnectAndSend(ipp.IP,ipp.Port,("t2"+IP+':'+QString::number(Port)).toUtf8());
            Log(QString("%1:%2 requested the address of %3(%4:%5)").arg(IP).arg(Port).arg(QString(Data.mid(2))).arg(ipp.IP).arg(ipp.Port));
        }
    } else if (Data[0]=='m') {
        for (QMap<QString,UserData>::iterator it=Clients.begin();it!=Clients.end();++it)
            if (it.value().IP=="127.0.0.1")
                protocol->ConnectAndSend("127.0.0.1",it.value().Port,Data);
        QStringList qsl(QString(Data.mid(1)).split('\n'));
        Log(QString("%1(%2:%3) said to server: %4").arg(qsl.at(0)).arg(IP).arg(Port).arg(qsl.at(1)));
    } else if (Data[0]=='f') {
        QString n(Data.mid(1,Data.indexOf('\n')-1));
        QMap<QString,UserData>::iterator it=Clients.find(n);
        QStringList qsl(QString(Data.mid(1)).split('\n'));
        if (it!=Clients.end()) {
            protocol->ConnectAndSend(it.value().IP,it.value().Port,'m'+Data.mid(Data.indexOf('\n')+1));
            Log(QString("%1(%2:%3) said to %4(%5:%6): %7").arg(qsl.at(1)).arg(IP).arg(Port).arg(qsl.at(0)).arg(it.value().IP).arg(it.value().Port).arg(qsl.at(2)));
        } else {
            Log(QString("%1(%2:%3) failed to say to %4: %5").arg(qsl.at(1)).arg(IP).arg(Port).arg(qsl.at(0)).arg(qsl.at(2)));
        }
    } else if (Data[0]=='b') {
        for (QMap<QString,UserData>::iterator it=Clients.begin();it!=Clients.end();++it)
            if (it.value().IP!=IP||it.value().Port!=Port)
                protocol->ConnectAndSend(it.value().IP,it.value().Port,Data);
        QStringList qsl(QString(Data.mid(1)).split('\n'));
        Log(QString("%1(%2:%3) broadcast: %4").arg(qsl.at(0)).arg(IP).arg(Port).arg(qsl.at(1)));
    } else if (Data.left(2)=="c0") {
        QString n(Data.mid(2));
        QMap<QString,UserData>::iterator it=Clients.find(n);
        if (it!=Clients.end()&&it.value().IP==IP&&it.value().Port==Port&&it.value().Cloak==false) {
            it.value().Cloak=true;
            protocol->ConnectAndSend(IP,Port,"c1");
            Log(QString("%1(%2:%3) cloaked").arg(QString(Data.mid(2))).arg(IP).arg(Port));
        } else {
            Log(QString("%1:%2 faked the cloak of %3 but failed").arg(IP).arg(Port).arg(QString(Data.mid(2))));
        }
    } else if (Data.left(2)=="c2") {
        QString n(Data.mid(2));
        QMap<QString,UserData>::iterator it=Clients.find(n);
        if (it!=Clients.end()&&it.value().IP==IP&&it.value().Port==Port&&it.value().Cloak==true) {
            it.value().Cloak=false;
            protocol->ConnectAndSend(IP,Port,"c3");
            Log(QString("%1(%2:%3) decloaked").arg(QString(Data.mid(2))).arg(IP).arg(Port));
        } else {
            Log(QString("%1:%2 faked the decloak of %3 but failed").arg(IP).arg(Port).arg(QString(Data.mid(2))));
        }
    }
}

void Server::RemoveClient(const QString &Nickname) {
    QMap<QString,UserData>::iterator it=Clients.find(Nickname);
    if (it!=Clients.end()) {
        Log(QString("%1(%2:%3) is forced to log off").arg(Nickname).arg(it.value().IP).arg(it.value().Port));
        Clients.remove(Nickname);
    } else {
        Log(QString("%1 is not found").arg(Nickname));
    }
}
