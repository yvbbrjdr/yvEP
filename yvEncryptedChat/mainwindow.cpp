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
    remaintimer=new QTimer(this);
    setWindowTitle(windowTitle()+" - "+Nickname+'@'+ServerIP+':'+QString::number(ServerPort));
    DownLabel=new QLabel(this);
    ui->statusBar->addWidget(DownLabel,1000);
    RemainLabel=new QLabel(this);
    ui->statusBar->addWidget(RemainLabel,1);
    ui->statusBar->setStyleSheet("QStatusBar::item{border: 0px}");
    Notification=new QMediaPlayer(this);
    Notification->setMedia(QUrl::fromLocalFile(QApplication::applicationDirPath()+"/notification.wav"));
    Notification->setVolume(100);
    pm=new PluginManager;
    connect(pm,SIGNAL(SendMsg(QString,QString)),this,SLOT(SendMsg(QString,QString)));
    connect(ui->Message,SIGNAL(returnPressed()),this,SLOT(SendMessage()));
    connect(ui->CloakButton,SIGNAL(clicked(bool)),this,SLOT(Cloak()));
    connect(refreshtimer,SIGNAL(timeout()),this,SLOT(Refresh()));
    connect(remaintimer,SIGNAL(timeout()),this,SLOT(RefreshRemain()));
    connect(ui->ClientList,SIGNAL(clicked(QModelIndex)),this,SLOT(Touch(QModelIndex)));
    connect(ui->ServerForward,SIGNAL(clicked(bool)),this,SLOT(ForwardCheck()));
    connect(ui->ClearHistory,SIGNAL(clicked(bool)),this,SLOT(ClearHistory()));
    connect(ui->BroadcastList,SIGNAL(clicked(bool)),this,SLOT(BroadcastListClick()));
    connect(ui->PluginsButton,SIGNAL(clicked(bool)),pm,SLOT(ShowPluginsControlPanel()));
    ui->BroadcastList->setVisible(false);
    ConfigManager config;
    QVariantMap qvm(config.GetConfig(QApplication::applicationDirPath()+"/yvEC.config"));
    ui->Prefix->setText(qvm["prefix"].toString());
    ui->Suffix->setText(qvm["suffix"].toString());
    refreshtimer->start(30000);
    remaintimer->start(1000);
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
        History[n]+="<p style=\"color:green;\">"+QTime::currentTime().toString("hh:mm:ss")+' '+n+"<br>"+Data["message"].toString()+"</p>";
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
        emit pm->RecvMsg(Data["nickname"].toString(),Data["message"].toString());
    } else if (Data["type"]=="broadcast") {
        History["Broadcast"]+="<p style=\"color:green;\">"+QTime::currentTime().toString("hh:mm:ss")+' '+Data["nickname"].toString()+"<br>"+Data["message"].toString()+"</p>";
        if (RemoteNickname=="Broadcast") {
            ui->History->setHtml(History["Broadcast"]);
            CursorDown();
        }
        emit pm->RecvMsg("Broadcast",Data["message"].toString());
    } else if (Data["type"]=="cloak") {
        if (Data["status"]=="cloaked") {
            Cloaking=1;
            ui->CloakButton->setChecked(true);
        } else {
            Cloaking=0;
            ui->CloakButton->setChecked(false);
        }
    } else if (Data["type"]=="logout") {
        QApplication::quit();
    } else if (Data["type"]=="info") {
        ui->History->setHtml(Data["content"].toString());
        Refresh();
    } else if (Data["type"]=="forward") {
        QMessageBox::critical(this,"ERROR","The message wasn't sent out.\nThe user has logged out.");
    } else if (Data["type"]=="punch") {
        if (Data["num"].toInt()==0) {
            QVariantMap punch;
            punch["type"]="punch";
            punch["num"]=1;
            protocol->SendData(RemoteIP,RemotePort,punch);
        }
    }
}

void MainWindow::SendMessage() {
    if (ui->Message->text()=="")
        return;
    QString Message(ui->Prefix->text()+ui->Message->text()+ui->Suffix->text());
    Message.replace('\n',"<br>");
    QVariantMap qvm;
    qvm["nickname"]=Nickname;
    qvm["message"]=Message;
    if (RemoteNickname=="Broadcast") {
        qvm["type"]="broadcast";
        if (Cloaking)
            qvm["nickname"]="Cloaked";
        if (BroadcastList!="") {
            qvm["to"]=BroadcastList;
            Message+="<br>(To "+BroadcastList+')';
            qvm["message"]=Message;
        }
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

    History[RemoteNickname]+="<p style=\"text-align:right;color:blue;\">"+QTime::currentTime().toString("hh:mm:ss")+' '+Nickname+"<br>"+Message+"</p>";
    ui->History->setHtml(History[RemoteNickname]);
    CursorDown();
    ui->Message->setText("");
}

void MainWindow::Refresh() {
    if ((!ui->ServerForward->isChecked())&&RemoteNickname!=""&&RemoteNickname!=Nickname&&RemoteNickname!="Broadcast") {
        QVariantMap punch;
        punch["type"]="punch";
        punch["num"]=0;
        protocol->SendData(RemoteIP,RemotePort,punch);
    }
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
    ConfigManager config;
    qvm=config.GetConfig(QApplication::applicationDirPath()+"/yvEC.config");
    qvm["prefix"]=ui->Prefix->text();
    qvm["suffix"]=ui->Suffix->text();
    config.SetConfig(QApplication::applicationDirPath()+"/yvEC.config",qvm);
    QTime t=QTime::currentTime();
    while (t.msecsTo(QTime::currentTime())<=100);
    delete pm;
}

void MainWindow::Touch(const QModelIndex &index) {
    ui->History->setHtml(History[index.data().toString()]);
    CursorDown();
    if (index.data().toString()=="Broadcast") {
        ui->BroadcastList->setVisible(true);
        RemoteNickname="Broadcast";
        ui->Message->setEnabled(true);
        ui->Message->setFocus();
        return;
    }
    ui->BroadcastList->setVisible(false);
    ui->Message->setEnabled(false);
    if (index.data().toString()==Nickname) {
        RemoteNickname=Nickname;
        if (DownLabel->text().left(DownLabel->text().length()-20)==RemoteNickname)
            DownLabel->setText("");
        QMessageBox::warning(this,"WARNING","You can't chat with yourself!");
        return;
    }
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
    ui->CloakButton->setChecked(!ui->CloakButton->isChecked());
    QVariantMap qvm;
    qvm["type"]="cloak";
    qvm["nickname"]=Nickname;
    protocol->SendData(ServerIP,ServerPort,qvm);
}

void MainWindow::ForwardCheck() {
    if (ui->ServerForward->isChecked()&&QMessageBox::warning(this,"WARNING","Server Forwarding is a dangerous operation that the server could see your messages.\nAre you sure to continue?",QMessageBox::Yes,QMessageBox::No)==QMessageBox::No)
        ui->ServerForward->setChecked(false);
}

void MainWindow::ClearHistory() {
    History[RemoteNickname]="";
    ui->History->setHtml("");
}

void MainWindow::Failed(const QString &IP,unsigned short Port) {
    if (IP==ServerIP&&Port==ServerPort) {
        if (QMessageBox::critical(this,"ERROR","Something wrong happens.\nThe connection to the server is down.\nDo you want to reconnect?",QMessageBox::Yes,QMessageBox::No)==QMessageBox::Yes) {
            protocol->ConnectTo(ServerIP,ServerPort);
        } else {
            ui->CloakButton->setEnabled(false);
            ui->ClientList->setEnabled(false);
            ui->BroadcastList->setEnabled(false);
            ui->ServerForward->setChecked(false);
            ui->ServerForward->setEnabled(false);
        }
    } else {
        QMessageBox::critical(this,"ERROR","Something wrong happens.\nThe connection to the user is down. ");
    }
}

void MainWindow::SendMsg(const QString &Nick,const QString &Content) {
    QString Message(Content);
    Message.replace('\n',"<br>");
    History[Nick]+="<p style=\"text-align:right\"><font color=\"blue\">"+QTime::currentTime().toString("hh:mm:ss")+' '+Nickname+"<br>"+Message.replace('\n',"<br>")+"</font></p>";
    if (Nick==RemoteNickname) {
        ui->History->setHtml(History[Nick]);
        CursorDown();
    }
    QVariantMap qvm;
    qvm["nickname"]=Nickname;
    qvm["message"]=Content;
    if (Nick!="Broadcast") {
        qvm["type"]="forward";
        qvm["to"]=Nick;
    } else {
        qvm["type"]="broadcast";
    }
    protocol->SendData(ServerIP,ServerPort,qvm);
}

void MainWindow::RefreshRemain() {
    RemainLabel->setText(QString("%1 Byte(s)").arg(protocol->BufferRemain()));
}

void MainWindow::BroadcastListClick() {
    bool ok;
    QString tmp(QInputDialog::getMultiLineText(this,"Broadcast List","Enter the users you want to broadcast to (One line per each) (Leave empty for everyone)",BroadcastList,&ok));
    if (ok)
        BroadcastList=tmp;
}
