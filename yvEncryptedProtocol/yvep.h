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
#include <QThread>
#include <QMap>
#include <QPair>
#include <QString>
#include <QVariantMap>
#include <QJsonDocument>
#include <sodium.h>
#include "statusflag.h"
#include "yvepsocket.h"
#include "udpsocket.h"

class YVENCRYPTEDPROTOCOLSHARED_EXPORT yvEP : public QObject {
    Q_OBJECT
private:
    UdpSocket *socket;
    QThread *socketthread;
    typedef QPair<QString,unsigned int> Addr;
    QMap<Addr,yvEPSocket*>Remotes;
public:
    QByteArray PublicKey,PrivateKey;
    explicit yvEP(QObject *parent = 0);
    ~yvEP();
    bool Bind(unsigned int Port);
    bool ConnectTo(const QString &IP,unsigned short Port);
    bool SendData(const QString &IP,unsigned short Port,const QVariantMap &Data);
    unsigned long BufferRemain();
    void SendRaw(const QString &IP,unsigned short Port,const QByteArray &Data);
    void ResetRemote(const QString &IP,unsigned short Port);
signals:
    void Connected(const QString &IP,unsigned short Port);
    void RecvData(const QString &IP,unsigned short Port,const QVariantMap &Data);
    void Reset(const QString &IP,unsigned short Port);
private slots:
    void ProcessRaw(const QString &IP,unsigned short Port,const QByteArray &Data);
};

#endif // YVEP_H
