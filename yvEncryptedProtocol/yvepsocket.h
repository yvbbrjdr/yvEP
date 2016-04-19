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

#ifndef YVEPSOCKET_H
#define YVEPSOCKET_H

#include <QObject>
#include <QVariantMap>
#include <QByteArray>
#include <QJsonDocument>
#include "socketbuffer.h"
#include "socketreceiver.h"
#include "socketsender.h"

class yvEP;

class yvEPSocket : public QObject {
    Q_OBJECT
private:
    QString IP;
    unsigned short Port;
    yvEP *protocol;
public:
    SocketBuffer *buffer;
private:
    SocketSender *sender;
    SocketReceiver *receiver;
public:
    explicit yvEPSocket(const QString &IP,unsigned short Port,yvEP *protocol,QObject *parent = 0);
    void ProcessRaw(const QByteArray &Data);
    void PushData(const QVariantMap &Data);
    ~yvEPSocket();
private slots:
    void SendRaw(const QByteArray &Data);
    void RecvData(const QVariantMap &Data);
    void Reset();
};

#include "yvep.h"

#endif // YVEPSOCKET_H
