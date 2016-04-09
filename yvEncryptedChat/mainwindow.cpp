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
    connect(protocol,SIGNAL(RecvData(QString,unsigned short,QByteArray)),this,SLOT(RecvData(QString,unsigned short,QByteArray)));
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

void MainWindow::RecvData(const QString&,unsigned short,const QByteArray &Data) {
    if (Data.left(2)=="li") {
        QStringList Clients;
        Clients.append("Broadcast");
        if (Data.size()!=2)
            Clients.append(QString(Data.mid(3)).split('\n'));
        listmodel->setStringList(Clients);
        ui->ClientList->setModel(listmodel);
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
        protocol->SendData(qsl.at(0),qsl.at(1).toInt(),"hello");
    } else if (Data.left(2)=="t3") {
        Refresh();
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
        if (Notification->isAudioAvailable()) {
            Notification->stop();
            Notification->play();
        }
    } else if (Data[0]=='b') {
        if (Data.mid(1,Data.indexOf('\n')-1)==Nickname)
            return;
        History["Broadcast"]+="<p style=\"text-align:left\"><font color=\"green\">"+QTime::currentTime().toString("hh:mm:ss")+' '+Data.mid(1).replace('\n',"<br>")+"</font></p>";
        if (RemoteNickname=="Broadcast") {
            ui->History->setHtml(History["Broadcast"]);
            CursorDown();
        }
    } else if (Data.left(2)=="c1") {
        Cloaking=1;
        ui->CloakButton->setText("Decloak");
    } else if (Data.left(2)=="c2") {
        Cloaking=0;
        ui->CloakButton->setText("Cloak");
    } else if (Data.left(2)=="l3") {
        QApplication::quit();
    } else if (Data.left(2)=="f0") {
        QMessageBox::critical(this,"Error","Failed to deliver this message\n- The user has logged off");
    } else if (Data.left(2)=="f1") {
        QMessageBox::critical(this,"Error","Failed to deliver this message\nPossible reasons:\n- The package is missing\n- The user has logged off\nTry again");
    }
}

void MainWindow::SendMessage() {
    if (ui->Message->text()=="")
        return;
    QString Message(Nickname+'\n'+ui->Message->text());
    if (RemoteNickname=="Broadcast"&&Cloaking)
        Message="Cloaked\n"+ui->Message->text();
    History[RemoteNickname]+="<p style=\"text-align:right\"><font color=\"blue\">"+QTime::currentTime().toString("hh:mm:ss")+' '+QString(Message).replace('\n',"<br>")+"</font></p>";
    ui->History->setHtml(History[RemoteNickname]);
    CursorDown();
    ui->Message->setText("");
    if (RemoteNickname=="Broadcast") {
        protocol->SendData(ServerIP,ServerPort,('b'+Message).toUtf8());
    } else if (ui->ServerForward->isChecked()) {
        if (!protocol->SendAndConfirm(ServerIP,ServerPort,('f'+RemoteNickname+'\n'+Message).toUtf8())) {
            QMessageBox::critical(this,"Error","Failed to deliver this message\nPossible reasons:\n- The package is missing\n- The server is shut down\nTry again");
        }
    } else {
        if (!protocol->SendAndConfirm(RemoteIP,RemotePort,('m'+Message).toUtf8())) {
            QMessageBox::critical(this,"Error","Failed to deliver this message\nPossible reasons:\n- UDP punching is disabled\n- The package is missing\n- The user has logged off\nTry again or use Server Forwarding");
        }
    }
}

void MainWindow::Refresh() {
    protocol->SendData(ServerIP,ServerPort,"li");
}

void MainWindow::closeEvent(QCloseEvent*) {
    protocol->SendData(ServerIP,ServerPort,("l3"+Nickname).toUtf8());
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
    protocol->SendData(ServerIP,ServerPort,("t0"+index.data().toString()).toUtf8());
}

void MainWindow::CursorDown() {
    QTextCursor qtc(ui->History->textCursor());
    qtc.movePosition(QTextCursor::End);
    ui->History->setTextCursor(qtc);
}

void MainWindow::Cloak() {
    protocol->SendData(ServerIP,ServerPort,("c0"+Nickname).toUtf8());
}

void MainWindow::ForwardCheck() {
    if (ui->ServerForward->isChecked()&&QMessageBox::warning(this,"WARNING","Server Forwarding is a dangerous operation that the server could see your messages.\nAre you sure to continue?",QMessageBox::Yes,QMessageBox::No)==QMessageBox::No)
        ui->ServerForward->setCheckState(Qt::Unchecked);
}

void MainWindow::ClearHistory() {
    History[RemoteNickname]="";
    ui->History->setHtml("");
}
