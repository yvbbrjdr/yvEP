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

MainWindow::MainWindow(yvEP *protocol,const QString &ServerIP,unsigned short ServerPort,const QString &Nickname,QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow),protocol(protocol),ServerIP(ServerIP),ServerPort(ServerPort),Nickname(Nickname),Cloaking(false) {
    connect(protocol,SIGNAL(RecvData(QString,unsigned short,QVariantMap)),this,SLOT(RecvData(QString,unsigned short,QVariantMap)));
    connect(protocol,SIGNAL(Reset(QString,unsigned short)),this,SLOT(Failed(QString,unsigned short)));
    ui->setupUi(this);
    listmodel=new QStringListModel(this);
    refreshtimer=new QTimer(this);
    setWindowTitle(windowTitle()+" - "+Nickname+'@'+ServerIP+':'+QString::number(ServerPort));
    DownLabel=new QLabel;
    DownLabel->setMinimumSize(DownLabel->sizeHint());
    DownLabel->setAlignment(Qt::AlignHCenter);
    ui->statusBar->addWidget(DownLabel);
    ui->statusBar->setStyleSheet("QStatusBar::item{border: 0px}");
    Notification=new QMediaPlayer(this);
    Notification->setMedia(QUrl::fromLocalFile(QApplication::applicationDirPath()+"/notification.wav"));
    Notification->setVolume(100);
    connect(ui->Message,SIGNAL(returnPressed()),this,SLOT(SendMessage()));
    connect(ui->RefreshButton,SIGNAL(clicked(bool)),this,SLOT(Refresh()));
    connect(ui->CloakButton,SIGNAL(clicked(bool)),this,SLOT(Cloak()));
    connect(refreshtimer,SIGNAL(timeout()),this,SLOT(Refresh()));
    connect(ui->ClientList,SIGNAL(clicked(QModelIndex)),this,SLOT(Touch(QModelIndex)));
    connect(ui->ServerForward,SIGNAL(stateChanged(int)),this,SLOT(ForwardCheck()));
    connect(ui->ClearHistory,SIGNAL(clicked(bool)),this,SLOT(ClearHistory()));
    refreshtimer->start(30000);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::RecvData(const QString&,unsigned short,const QVariantMap &Data) {
    if (Data["type"]=="list") {
        QStringList Clients;
        Clients.append("Broadcast");
        if (Data["list"]!="")
            Clients.append(Data["list"].toString().split('\n'));
        listmodel->setStringList(Clients);
        ui->ClientList->setModel(listmodel);
    } else if (Data["type"]=="touch") {
        if (Data["result"]=="1") {
            RemoteIP=Data["IP"].toString();
            RemotePort=Data["port"].toInt();
            RemoteNickname=Data["nickname"].toString();
            ui->Message->setEnabled(true);
            ui->Message->setFocus();
            if (DownLabel->text().left(DownLabel->text().length()-20)==RemoteNickname)
                DownLabel->setText("");
        } else {
            protocol->SendRaw(Data["IP"].toString(),Data["port"].toInt(),QByteArray(1,0));
        }
    } else if (Data["type"]=="refresh") {
        Refresh();
    } else if (Data["type"]=="message") {
        QString n=Data["nickname"].toString();
        History[n]+="<p style=\"text-align:left\"><font color=\"green\">"+QTime::currentTime().toString("hh:mm:ss")+' '+n+"<br>"+Data["message"].toString().replace('\n',"<br>")+"</font></p>";
        if (n==RemoteNickname) {
            ui->History->setHtml(History[n]);
            CursorDown();
        } else {
            DownLabel->setText(n+" sent you a message.");
        }
        activateWindow();
        if (Notification->isAudioAvailable()) {
            Notification->stop();
            Notification->play();
        }
    } else if (Data["type"]=="broadcast") {
        History["Broadcast"]+="<p style=\"text-align:left\"><font color=\"green\">"+QTime::currentTime().toString("hh:mm:ss")+' '+Data["nickname"].toString()+"<br>"+Data["message"].toString().replace('\n',"<br>")+"</font></p>";
        if (RemoteNickname=="Broadcast") {
            ui->History->setHtml(History["Broadcast"]);
            CursorDown();
        }
    } else if (Data["type"]=="cloak") {
        if (Data["status"]=="cloaked") {
            Cloaking=1;
            ui->CloakButton->setText("Decloak");
        } else {
            Cloaking=0;
            ui->CloakButton->setText("Cloak");
        }
    } else if (Data["type"]=="logout") {
        QApplication::quit();
    } else if (Data["type"]=="info") {
        ui->History->setHtml(Data["content"].toString());
        Refresh();
    }
}

void MainWindow::SendMessage() {
    if (ui->Message->text()=="")
        return;
    QString Message(ui->Prefix->text()+ui->Message->text()+ui->Suffix->text());
    History[RemoteNickname]+="<p style=\"text-align:right\"><font color=\"blue\">"+QTime::currentTime().toString("hh:mm:ss")+' '+Nickname+"<br>"+Message.replace('\n',"<br>")+"</font></p>";
    ui->History->setHtml(History[RemoteNickname]);
    CursorDown();
    ui->Message->setText("");
    QVariantMap qvm;
    qvm["nickname"]=Nickname;
    qvm["message"]=Message;
    if (RemoteNickname=="Broadcast") {
        qvm["type"]="broadcast";
        if (Cloaking)
            qvm["nickname"]="Cloaked";
        protocol->SendData(ServerIP,ServerPort,qvm);
    } else if (ui->ServerForward->isChecked()) {
        qvm["type"]="forward";
        qvm["to"]=RemoteNickname;
        protocol->SendData(ServerIP,ServerPort,qvm);
    } else {
        qvm["type"]="message";
        protocol->ConnectTo(RemoteIP,RemotePort);
        protocol->SendData(RemoteIP,RemotePort,qvm);
    }
}

void MainWindow::Refresh() {
    QVariantMap qvm;
    qvm["type"]="list";
    qvm["nickname"]=Nickname;
    protocol->SendData(ServerIP,ServerPort,qvm);
}

void MainWindow::closeEvent(QCloseEvent*) {
    QVariantMap qvm;
    qvm["type"]="logout";
    qvm["nickname"]=Nickname;
    protocol->SendData(ServerIP,ServerPort,qvm);
}

void MainWindow::Touch(const QModelIndex &index) {
    ui->History->setHtml(History[index.data().toString()]);
    CursorDown();
    if (index.data().toString()=="Broadcast") {
        RemoteNickname="Broadcast";
        ui->Message->setEnabled(true);
        ui->Message->setFocus();
        return;
    }
    ui->Message->setEnabled(false);
    QVariantMap qvm;
    qvm["type"]="touch";
    qvm["nickname"]=index.data().toString();
    protocol->SendData(ServerIP,ServerPort,qvm);
}

void MainWindow::CursorDown() {
    QTextCursor qtc(ui->History->textCursor());
    qtc.movePosition(QTextCursor::End);
    ui->History->setTextCursor(qtc);
}

void MainWindow::Cloak() {
    QVariantMap qvm;
    qvm["type"]="cloak";
    qvm["nickname"]=Nickname;
    protocol->SendData(ServerIP,ServerPort,qvm);
}

void MainWindow::ForwardCheck() {
    if (ui->ServerForward->isChecked()&&QMessageBox::warning(this,"WARNING","Server Forwarding is a dangerous operation that the server could see your messages.\nAre you sure to continue?",QMessageBox::Yes,QMessageBox::No)==QMessageBox::No)
        ui->ServerForward->setCheckState(Qt::Unchecked);
}

void MainWindow::ClearHistory() {
    History[RemoteNickname]="";
    ui->History->setHtml("");
}

void MainWindow::Failed(const QString &IP,unsigned short Port) {
    if (IP==ServerIP&&Port==ServerPort) {
        QMessageBox::critical(this,"ERROR","Something wrong happens.\nThe connection to the server is down.");
        protocol->ConnectTo(ServerIP,ServerPort);
    } else {
        QMessageBox::critical(this,"ERROR","Something wrong happens.\nThe message wasn't sent out.");
    }
}
