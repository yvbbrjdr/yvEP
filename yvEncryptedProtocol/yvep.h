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

#ifndef YVEP_H
#define YVEP_H

#include "yvencryptedprotocol_global.h"
#include <QObject>
#include <QByteArray>
#include <sodium.h>
#include <QString>
#include <QThread>
#include <QTime>
#include <QCoreApplication>
#include <QMap>
#include "udpsocket.h"

class YVENCRYPTEDPROTOCOLSHARED_EXPORT yvEP : public QObject {
    Q_OBJECT
public:
    explicit yvEP(unsigned short Port=0,QObject *parent=0);
    ~yvEP();
    bool Bound();
    QString CurRemoteIP();
    unsigned short CurRemotePort();
    bool ConnectTo(const QString &IP,unsigned short Port);
    bool SendData(const QByteArray &Data);
    bool ConnectAndSend(const QString &IP,unsigned short Port,const QByteArray &Data);
private:
    UdpSocket *socket;
    QThread *thread;
    QString RemoteIP;
    unsigned short RemotePort;
    QByteArray LocalPublicKey,LocalPrivateKey,RemotePublicKey;
    bool connecting;
    QMap<QPair<QString,unsigned short>,QByteArray>PublicKeys;
signals:
    void RecvData(const QString &IP,unsigned short Port,const QByteArray &Data);
    void ConnectYou(const QString &IP,unsigned short Port);
private slots:
    void ProcessData(const QString &IP,unsigned short Port,const QByteArray &Data);
};

#endif // YVEP_H
