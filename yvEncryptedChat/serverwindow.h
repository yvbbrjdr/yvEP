/*
This file is part of yvEncryptedChat
yvEncryptedChat is a IM program that provides secure chats. 
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

#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QMainWindow>
#include <yvep.h>
#include <QThread>
#include <QMap>
#include <QPair>
#include <QStringList>
#include <QStringListModel>

namespace Ui {
    class ServerWindow;
}

class ServerWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit ServerWindow(yvEP* protocol,QWidget *parent=0);
    ~ServerWindow();
private:
    Ui::ServerWindow *ui;
    yvEP *protocol;
    QMap<QString,QPair<QString,unsigned short> >Clients;
    QStringListModel *listmodel;
    void UpdateList();
private slots:
    void RecvData(const QString &IP,unsigned short Port,const QByteArray &Data);
    void RemoveClient(const QModelIndex &index);
};

#endif // SERVERWINDOW_H
