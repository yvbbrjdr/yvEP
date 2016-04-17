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

Server::Server() {
    protocol=new yvEP(this);
    connect(protocol,SIGNAL(RecvData(QString,unsigned short,QVariantMap)),this,SLOT(RecvData(QString,unsigned short,QVariantMap)));
    QFile info(QCoreApplication::applicationDirPath()+"/yvECS.info");
    if (info.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream qts(&info);
        Info=qts.readAll();
    }
}

void Server::Log(const QString &Content) {
    fprintf(stderr,"[%s] %s\n",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toUtf8().data(),Content.toUtf8().data());
}

void Server::RecvData(const QString &IP,unsigned short Port,const QVariantMap &Data) {
    QVariantMap qvm;
    if (Data["type"]=="login") {
        if (Clients.find(Data["nickname"].toString())==Clients.end()&&Data["nickname"]!="Broadcast") {
            Log(QString("%1:%2 logged in as nickname %3").arg(IP).arg(Port).arg(Data["nickname"].toString()));
            qvm["type"]="login";
            qvm["result"]="success";
            protocol->SendData(IP,Port,qvm);
            qvm.clear();
            qvm["type"]="refresh";
            for (QMap<QString,UserData>::iterator it=Clients.begin();it!=Clients.end();++it)
                protocol->SendData(it.value().IP,it.value().Port,qvm);
            Clients.insert(Data["nickname"].toString(),UserData(IP,Port));
            qvm.clear();
            qvm["type"]="info";
            qvm["content"]=Info;
            protocol->SendData(IP,Port,qvm);
        } else {
            qvm["type"]="login";
            qvm["result"]="fail";
            protocol->SendData(IP,Port,qvm);
            Log(QString("%1:%2 tried to log in as nickname %3 but failed").arg(IP).arg(Port).arg(Data["nickname"].toString()));
        }
    } else if (Data["type"]=="list") {
        QString s;
        for (QMap<QString,UserData>::iterator it=Clients.begin();it!=Clients.end();++it) {
            if (it.value().Cloak||it.key()==Data["nickname"].toString())
                continue;
            s+=it.key()+'\n';
        }
        s=s.left(s.length()-1);
        qvm["type"]="list";
        qvm["list"]=s;
        protocol->SendData(IP,Port,qvm);
    } else if (Data["type"]=="logout") {
        QString n(Data["nickname"].toString());
        QMap<QString,UserData>::iterator it=Clients.find(n);
        if (it!=Clients.end()&&it.value().IP==IP&&it.value().Port==Port) {
            Clients.erase(it);
            Log(QString("%1(%2:%3) logged out").arg(n).arg(IP).arg(Port));
            qvm["type"]="refresh";
            for (QMap<QString,UserData>::iterator it=Clients.begin();it!=Clients.end();++it)
                protocol->SendData(it.value().IP,it.value().Port,qvm);
        }
    } else if (Data["type"]=="touch") {
        if (Clients.find(Data["nickname"].toString())==Clients.end()||Clients.find(Data["nickname"].toString()).value().Cloak) {
            qvm["type"]="refresh";
            protocol->SendData(IP,Port,qvm);
            Log(QString("%1:%2 requested the address of %3 but failed").arg(IP).arg(Port).arg(Data["nickname"].toString()));
        } else {
            UserData ipp=Clients.find(Data["nickname"].toString()).value();
            qvm["type"]="touch";
            qvm["result"]="1";
            qvm["IP"]=ipp.IP;
            qvm["port"]=ipp.Port;
            qvm["nickname"]=Data["nickname"];
            protocol->SendData(IP,Port,qvm);
            qvm.clear();
            qvm["type"]="touch";
            qvm["result"]="2";
            qvm["IP"]=IP;
            qvm["port"]=Port;
            protocol->SendData(ipp.IP,ipp.Port,qvm);
            Log(QString("%1:%2 requested the address of %3(%4:%5)").arg(IP).arg(Port).arg(Data["nickname"].toString()).arg(ipp.IP).arg(ipp.Port));
        }
    } else if (Data["type"]=="forward") {
        QString n(Data["to"].toString());
        QMap<QString,UserData>::iterator it=Clients.find(n);
        if (it!=Clients.end()) {
            qvm["type"]="message";
            qvm["nickname"]=Data["nickname"];
            qvm["message"]=Data["message"];
            protocol->SendData(it.value().IP,it.value().Port,qvm);
        } else {
            qvm["type"]="forward";
            qvm["status"]="fail";
            protocol->SendData(IP,Port,qvm);
        }
    } else if (Data["type"]=="broadcast") {
        for (QMap<QString,UserData>::iterator it=Clients.begin();it!=Clients.end();++it)
            if (it.value().IP!=IP||it.value().Port!=Port)
                protocol->SendData(it.value().IP,it.value().Port,Data);
    } else if (Data["type"]=="cloak") {
        QString n(Data["nickname"].toString());
        QMap<QString,UserData>::iterator it=Clients.find(n);
        if (it!=Clients.end()&&it.value().IP==IP&&it.value().Port==Port) {
            it.value().Cloak=!it.value().Cloak;
            if (it.value().Cloak) {
                qvm["type"]="cloak";
                qvm["status"]="cloaked";
                protocol->SendData(IP,Port,qvm);
                Log(QString("%1(%2:%3) cloaked").arg(Data["nickname"].toString()).arg(IP).arg(Port));
            } else {
                qvm["type"]="cloak";
                qvm["status"]="decloaked";
                protocol->SendData(IP,Port,qvm);
                Log(QString("%1(%2:%3) decloaked").arg(Data["nickname"].toString()).arg(IP).arg(Port));
            }
            qvm.clear();
            qvm["type"]="refresh";
            for (QMap<QString,UserData>::iterator it=Clients.begin();it!=Clients.end();++it)
                protocol->SendData(it.value().IP,it.value().Port,qvm);
        }
    }
}

void Server::RemoveClient(const QString &Nickname) {
    QMap<QString,UserData>::iterator it=Clients.find(Nickname);
    if (it!=Clients.end()) {
        Log(QString("%1(%2:%3) is forced to log off").arg(Nickname).arg(it.value().IP).arg(it.value().Port));
        QVariantMap qvm;
        qvm["type"]="logout";
        protocol->SendData(it.value().IP,it.value().Port,qvm);
        Clients.remove(Nickname);
        qvm.clear();
        qvm["type"]="refresh";
        for (QMap<QString,UserData>::iterator it=Clients.begin();it!=Clients.end();++it)
            protocol->SendData(it.value().IP,it.value().Port,qvm);
    } else {
        Log(QString("%1 is not found").arg(Nickname));
    }
}
