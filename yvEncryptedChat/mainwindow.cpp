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

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(yvEP *protocol,const QString &ServerIP,unsigned short ServerPort,const QString &Nickname,QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow),protocol(protocol),ServerIP(ServerIP),ServerPort(ServerPort),Nickname(Nickname) {
    ui->setupUi(this);
    listmodel=new QStringListModel(this);
    refreshtimer=new QTimer(this);
    DownLabel=new QLabel;
    DownLabel->setMinimumSize(DownLabel->sizeHint());
    DownLabel->setAlignment(Qt::AlignHCenter);
    ui->statusBar->addWidget(DownLabel);
    ui->statusBar->setStyleSheet("QStatusBar::item{border: 0px}");
    connect(protocol,SIGNAL(RecvData(QString,unsigned short,QByteArray)),this,SLOT(RecvData(QString,unsigned short,QByteArray)));
    connect(ui->Message,SIGNAL(returnPressed()),this,SLOT(SendMessage()));
    connect(ui->RefreshButton,SIGNAL(clicked(bool)),this,SLOT(Refresh()));
    connect(ui->CloakButton,SIGNAL(clicked(bool)),this,SLOT(Cloak()));
    connect(refreshtimer,SIGNAL(timeout()),this,SLOT(Refresh()));
    connect(ui->ClientList,SIGNAL(clicked(QModelIndex)),this,SLOT(Touch(QModelIndex)));
    Refresh();
    refreshtimer->start(30000);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::RecvData(const QString&,unsigned short,const QByteArray &Data) {
    if (Data.left(2)=="li") {
        if (Data.size()==2)
            return;
        Clients.append(QString(Data.mid(3)).split('\n'));
        UpdateClients();
    } else if (Data.left(2)=="t1") {
        QStringList qsl=QString(Data.mid(2)).split(":");
        RemoteIP=qsl.at(0);
        RemotePort=qsl.at(1).toInt();
        RemoteNickname=qsl.at(2);
        ui->Message->setEnabled(true);
        ui->Message->setFocus();
        if (DownLabel->text().left(DownLabel->text().length()-20)==RemoteNickname)
            DownLabel->setText("");
    } else if (Data.left(2)=="t2") {
        QStringList qsl=QString(Data.mid(2)).split(":");
        protocol->ConnectTo(qsl.at(0),qsl.at(1).toInt());
    } else if (Data.left(2)=="t3") {
        Refresh();
        ui->Message->setEnabled(false);
    } else if (Data[0]=='m') {
        QString n=Data.mid(1,Data.indexOf('\n')-1);
        History[n]+="<p style=\"text-align:left\"><font color=\"green\">"+QTime::currentTime().toString("hh:mm:ss")+' '+Data.mid(1).replace('\n',"<br>")+"</font></p>";
        if (n==RemoteNickname) {
            ui->History->setHtml(History[n]);
            CursorDown();
        } else {
            DownLabel->setText(n+" sent you a message.");
        }
        activateWindow();
    }
}

void MainWindow::SendMessage() {
    if (ui->Message->text()=="")
        return;
    QString Message('m'+Nickname+'\n'+ui->Message->text());
    History[RemoteNickname]+="<p style=\"text-align:right\"><font color=\"blue\">"+QTime::currentTime().toString("hh:mm:ss")+' '+Message.mid(1).replace('\n',"<br>")+"</font></p>";
    ui->History->setHtml(History[RemoteNickname]);
    CursorDown();
    ui->Message->setText("");
    protocol->ConnectAndSend(RemoteIP,RemotePort,Message.toUtf8());
}

void MainWindow::Refresh() {
    Clients.clear();
    UpdateClients();
    protocol->ConnectAndSend(ServerIP,ServerPort,"li");
}

void MainWindow::closeEvent(QCloseEvent*) {
    protocol->ConnectAndSend(ServerIP,ServerPort,("l3"+Nickname).toUtf8());
}

void MainWindow::Touch(const QModelIndex &index) {
    ui->History->setHtml(History[index.data().toString()]);
    CursorDown();
    protocol->ConnectAndSend(ServerIP,ServerPort,("t0"+index.data().toString()).toUtf8());
}

void MainWindow::CursorDown() {
    QTextCursor qtc(ui->History->textCursor());
    qtc.movePosition(QTextCursor::End);
    ui->History->setTextCursor(qtc);
}

void MainWindow::UpdateClients() {
    listmodel->setStringList(Clients);
    ui->ClientList->setModel(listmodel);
}

void MainWindow::Cloak() {
    protocol->ConnectAndSend(ServerIP,ServerPort,("l4"+Nickname).toUtf8());
    Refresh();
}
