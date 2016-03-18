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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QMessageBox>
#include <yvep.h>
#include <QStringList>
#include <QStringListModel>
#include <QTimer>
#include <QMap>
#include <QTextCursor>
#include <QLabel>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(yvEP *protocol,const QString &Nickname,QWidget *parent=0);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    yvEP *protocol;
    QString ServerIP;
    unsigned short ServerPort;
    QString Nickname;
    QString RemoteIP;
    unsigned short RemotePort;
    QString RemoteNickname;
    QStringList Clients;
    QStringListModel *listmodel;
    QTimer *refreshtimer;
    QMap<QString,QString>History;
    void CursorDown();
    QLabel *DownLabel;
    void UpdateClients();
private slots:
    void RecvData(const QString&,unsigned short,const QByteArray &Data);
    void SendMessage();
    void Refresh();
    void Cloak();
    void Touch(const QModelIndex &index);
protected:
    void closeEvent(QCloseEvent*);
};

#endif // MAINWINDOW_H
