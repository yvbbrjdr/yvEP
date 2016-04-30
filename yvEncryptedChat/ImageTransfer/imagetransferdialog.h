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

#ifndef IMAGETRANSFERDIALOG_H
#define IMAGETRANSFERDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QBuffer>
#include "../pluginmanager.h"
#include "imageshowdialog.h"

namespace Ui {
    class ImageTransferDialog;
}

class ImageTransferDialog : public QDialog {
    Q_OBJECT
public:
    explicit ImageTransferDialog(PluginManager *manager);
    ~ImageTransferDialog();
private:
    Ui::ImageTransferDialog *ui;
signals:
    void SendMsg(const QString &Nickname,const QString &Content);
private slots:
    void RecvMsg(const QString &Nickname,const QString &Content);
    void Transfer();
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // IMAGETRANSFERDIALOG_H
