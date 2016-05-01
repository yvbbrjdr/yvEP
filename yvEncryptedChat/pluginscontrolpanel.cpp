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

#include "pluginscontrolpanel.h"
#include "ui_pluginscontrolpanel.h"

PluginsControlPanel::PluginsControlPanel(const QMap<QString,Plugin*>&instances,PluginManager *manager,QWidget *parent):QDialog(parent),ui(new Ui::PluginsControlPanel),instances(instances),manager(manager) {
    ui->setupUi(this);
    qslm=new QStringListModel(this);
    QStringList qsl;
    for (QMap<QString,Plugin*>::const_iterator it=instances.begin();it!=instances.end();++it)
        qsl.push_back(it.key());
    qslm->setStringList(qsl);
    ui->listView->setModel(qslm);
    connect(ui->listView,SIGNAL(clicked(QModelIndex)),this,SLOT(ListClick(QModelIndex)));
    connect(ui->Activate,SIGNAL(clicked(bool)),this,SLOT(Activate()));
    connect(ui->Deactivate,SIGNAL(clicked(bool)),this,SLOT(Deactivate()));
    connect(ui->Function,SIGNAL(clicked(bool)),this,SLOT(Function()));
}

PluginsControlPanel::~PluginsControlPanel() {
    delete ui;
}

void PluginsControlPanel::Activate() {
    Plugin* p=instances.find(ui->listView->selectionModel()->selectedIndexes().first().data().toString()).value();
    p->Init(manager);
    p->Activated=true;
    ListClick(ui->listView->selectionModel()->selectedIndexes().first());

}

void PluginsControlPanel::Deactivate() {
    Plugin* p=instances.find(ui->listView->selectionModel()->selectedIndexes().first().data().toString()).value();
    p->Destroy(manager);
    p->Activated=false;
    ListClick(ui->listView->selectionModel()->selectedIndexes().first());
}

void PluginsControlPanel::Function() {
    Plugin* p=instances.find(ui->listView->selectionModel()->selectedIndexes().first().data().toString()).value();
    p->Function();
    ListClick(ui->listView->selectionModel()->selectedIndexes().first());
}

void PluginsControlPanel::ListClick(const QModelIndex &qmi) {
    Plugin* p=instances.find(qmi.data().toString()).value();
    if (p->Activated) {
        ui->Activate->setEnabled(false);
        ui->Deactivate->setEnabled(true);
        if (p->FunctionName=="") {
            ui->Function->setText("Function");
            ui->Function->setEnabled(false);
        } else {
            ui->Function->setText(p->FunctionName);
            ui->Function->setEnabled(true);
        }
    } else {
        ui->Activate->setEnabled(true);
        ui->Deactivate->setEnabled(false);
        ui->Function->setEnabled(false);
        if (p->FunctionName=="") {
            ui->Function->setText("Function");
        } else {
            ui->Function->setText(p->FunctionName);
        }
    }
}
