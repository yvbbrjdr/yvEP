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

#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent):QDialog(parent),ui(new Ui::LoginDialog),protocol(NULL) {
    ui->setupUi(this);
    ui->Address->setFocus();
    QFile config(QApplication::applicationDirPath()+"/yvEC.config");
    if (config.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream stream(&config);
        QVariantMap qvm(QJsonDocument::fromJson(stream.readAll().toUtf8()).toVariant().toMap());
        ui->Address->setText(qvm["addr"].toString());
        ui->Port->setText(qvm["port"].toString());
        ui->Nickname->setText(qvm["name"].toString());
        config.close();
        ui->LoginButton->setFocus();
    }
    connect(ui->LoginButton,SIGNAL(clicked(bool)),this,SLOT(LoginPressed()));
    protocol=new yvEP;
    if (!protocol->LoadKey(QApplication::applicationDirPath()+"/yvEC.key")) {
        protocol->GenerateKey();
        protocol->SaveKey(QApplication::applicationDirPath()+"/yvEC.key");
    }
    connect(protocol,SIGNAL(RecvData(QString,unsigned short,QByteArray)),this,SLOT(RecvData(QString,unsigned short,QByteArray)));
}

LoginDialog::~LoginDialog() {
    delete ui;
}

void LoginDialog::LoginPressed() {
    if (ui->Address->text()==""||ui->Port->text()==""||ui->Nickname->text()=="") {
        QMessageBox::aboutQt(this,"About Qt");
        return;
    }
    ui->LoginButton->setEnabled(false);
    QVariantMap qvm;
    qvm.insert("addr",ui->Address->text());
    qvm.insert("port",ui->Port->text().toInt());
    qvm.insert("name",ui->Nickname->text());
    QFile config(QApplication::applicationDirPath()+"/yvEC.config");
    config.open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream stream(&config);
    stream<<QJsonDocument::fromVariant(qvm).toJson();
    config.close();
    ui->TitleLabel->setText("Connecting");
    QApplication::processEvents();
    ServerIP=ui->Address->text();
    if (!ServerIP.at(0).isDigit())
        ServerIP=QHostInfo::fromName(ServerIP).addresses().first().toString();
    if (protocol->SendData(ServerIP,ui->Port->text().toInt(),("l0"+ui->Nickname->text()).toUtf8())) {
        ui->TitleLabel->setText("Connected");
        QApplication::processEvents();
    } else {
        ui->TitleLabel->setText("Failed");
        ui->LoginButton->setEnabled(true);
    }
}

void LoginDialog::RecvData(const QString&,unsigned short,const QByteArray &Data) {
    if (Data=="l1") {
        disconnect(protocol,SIGNAL(RecvData(QString,unsigned short,QByteArray)),this,SLOT(RecvData(QString,unsigned short,QByteArray)));
        MainWindow *w=new MainWindow(protocol,ServerIP,ui->Port->text().toInt(),ui->Nickname->text());
        w->show();
        this->hide();
    } else if (Data=="l2") {
        QMessageBox::critical(this,"Failed","Someone else used this nickname:\n"+ui->Nickname->text()+"\nYou have to change one.");
        ui->LoginButton->setEnabled(true);
    }
}
