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
    ConfigManager config;
    QVariantMap qvm(config.GetConfig(QApplication::applicationDirPath()+"/yvEC.config"));
    if (!qvm.empty()) {
        ui->Address->setText(qvm["addr"].toString());
        ui->Port->setText(qvm["port"].toString());
        ui->Nickname->setText(qvm["name"].toString());
        ui->LoginButton->setFocus();
    }
    connect(ui->LoginButton,SIGNAL(clicked(bool)),this,SLOT(LoginPressed()));
    connect(ui->AboutButton,SIGNAL(clicked(bool)),this,SLOT(AboutPressed()));
    ad=new AboutDialog(this);
    protocol=new yvEP;
    protocol->Bind(0);
    connect(protocol,SIGNAL(RecvData(QString,unsigned short,QVariantMap)),this,SLOT(RecvData(QString,unsigned short,QVariantMap)));
    connect(protocol,SIGNAL(Reset(QString,unsigned short)),this,SLOT(Failed()));
}

LoginDialog::~LoginDialog() {
    delete ui;
}

void LoginDialog::LoginPressed() {
    if (ui->Address->text()==""||ui->Port->text()==""||ui->Nickname->text()=="") {
        QMessageBox::critical(this,"Error","Please complete the form");
        return;
    }
    if (ui->Nickname->text().indexOf('{')!=-1||ui->Nickname->text().indexOf('}')!=-1) {
        QMessageBox::critical(this,"Error","Invalid characters (\'\{',\'}\')");
        return;
    }
    ui->LoginButton->setEnabled(false);
    ConfigManager config;
    QVariantMap qvm(config.GetConfig(QApplication::applicationDirPath()+"/yvEC.config"));
    qvm.insert("addr",ui->Address->text());
    qvm.insert("port",ui->Port->text().toInt());
    qvm.insert("name",ui->Nickname->text());
    config.SetConfig(QApplication::applicationDirPath()+"/yvEC.config",qvm);
    ui->TitleLabel->setText("Connecting");
    QApplication::processEvents();
    ServerIP=ui->Address->text();
    if (!ServerIP.at(0).isDigit())
        ServerIP=QHostInfo::fromName(ServerIP).addresses().first().toString();
    protocol->ConnectTo(ServerIP,ui->Port->text().toInt());
    qvm.clear();
    qvm["type"]="login";
    qvm["nickname"]=ui->Nickname->text();
    protocol->SendData(ServerIP,ui->Port->text().toInt(),qvm);
}

void LoginDialog::RecvData(const QString&,unsigned short,const QVariantMap &Data) {
    if (Data["result"]=="success") {
        MainWindow *w=new MainWindow(protocol,ServerIP,ui->Port->text().toInt(),ui->Nickname->text());
        disconnect(protocol,SIGNAL(RecvData(QString,unsigned short,QVariantMap)),this,SLOT(RecvData(QString,unsigned short,QVariantMap)));
        disconnect(protocol,SIGNAL(Reset(QString,unsigned short)),this,SLOT(Failed()));
        w->show();
        close();
    } else if (Data["result"]=="fail") {
        QMessageBox::critical(this,"Failed","Someone else used this nickname:\n"+ui->Nickname->text()+"\nYou have to change one.");
        ui->LoginButton->setEnabled(true);
    }
}

void LoginDialog::Failed() {
    ui->TitleLabel->setText("Failed");
    ui->LoginButton->setEnabled(true);
}

void LoginDialog::AboutPressed() {
    ad->show();
}
