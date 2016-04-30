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

#include "imagetransferdialog.h"
#include "ui_imagetransferdialog.h"

ImageTransferDialog::ImageTransferDialog(PluginManager *manager):ui(new Ui::ImageTransferDialog) {
    ui->setupUi(this);
    connect(manager,SIGNAL(RecvMsg(QString,QString)),this,SLOT(RecvMsg(QString,QString)));
    connect(this,SIGNAL(SendMsg(QString,QString)),manager,SIGNAL(SendMsg(QString,QString)));
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(Transfer()));
}

ImageTransferDialog::~ImageTransferDialog() {
    delete ui;
}

void ImageTransferDialog::closeEvent(QCloseEvent *event) {
    event->ignore();
}

void ImageTransferDialog::RecvMsg(const QString &Nickname,const QString &Content) {
    if (Content.left(9)!="Image<!--")
        return;
    QPixmap Pixmap;
    Pixmap.loadFromData(QByteArray::fromBase64(Content.mid(9,Content.length()-12).toLocal8Bit()));
    ImageShowDialog *isd=new ImageShowDialog("Image from "+Nickname,Pixmap,NULL);
    isd->show();
    emit SendMsg(Nickname,"Image Received");
}

void ImageTransferDialog::Transfer() {
    if (ui->lineEdit->text()=="") {
        QMessageBox::critical(this,"Error","Please enter a nickname");
        return;
    }
    QString Filename=QFileDialog::getOpenFileName(this,"Select an image");
    if (Filename=="")
        return;
    QPixmap Pixmap;
    if (Pixmap.load(Filename)) {
        QByteArray qba;
        QBuffer buffer(&qba);
        buffer.open(QIODevice::WriteOnly);
        Pixmap.save(&buffer,"PNG");
        qba=qba.toBase64();
        if (QMessageBox::question(this,"Sure?",QString("Are you sure to transfer this image?\n[%1*%2] (%3 Bytes)").arg(Pixmap.width()).arg(Pixmap.height()).arg(qba.length()))==QMessageBox::No)
            return;
        emit SendMsg(ui->lineEdit->text(),QString("Sending image [%1*%2] (%3 Bytes)").arg(Pixmap.width()).arg(Pixmap.height()).arg(qba.length()));
        emit SendMsg(ui->lineEdit->text(),"Image<!--"+qba+"-->");
    } else
        QMessageBox::critical(this,"Error","Failed to load the image");
}
