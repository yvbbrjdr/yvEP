/*
This file is part of yvEncryptedChat
yvEncryptedChat is an IM program that provides secure chats. 
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

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QPluginLoader>
#include <QString>
#include <QVector>
#include <QDir>
#include <QCoreApplication>
#include "plugin.h"

class PluginManager : public QObject {
    Q_OBJECT
private:
    QVector<Plugin*>instances;
public:
    explicit PluginManager(QObject *parent = 0);
    ~PluginManager();
signals:
    void RecvMsg(const QString &Nickname,const QString &Content);
    void SendMsg(const QString &Nickname,const QString &Content);
};

#endif // PLUGINMANAGER_H
