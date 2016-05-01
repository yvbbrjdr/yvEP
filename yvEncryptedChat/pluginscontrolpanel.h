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

#ifndef PLUGINSCONTROLPANEL_H
#define PLUGINSCONTROLPANEL_H

#include <QDialog>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QStringListModel>

class Plugin;
class PluginManager;

namespace Ui {
    class PluginsControlPanel;
}

class PluginsControlPanel : public QDialog {
    Q_OBJECT
public:
    explicit PluginsControlPanel(const QMap<QString,Plugin*>&instances,PluginManager *manager,QWidget *parent = 0);
    ~PluginsControlPanel();
private:
    Ui::PluginsControlPanel *ui;
    QMap<QString,Plugin*>instances;
    QStringListModel *qslm;
    PluginManager *manager;
private slots:
    void Activate();
    void Deactivate();
    void Function();
    void ListClick(const QModelIndex &qmi);
};

#include "plugin.h"
#include "pluginmanager.h"

#endif // PLUGINSCONTROLPANEL_H
