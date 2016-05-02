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

#include "pluginmanager.h"

PluginManager::PluginManager(QObject *parent) : QObject(parent) {
    do {
        QDir dir(QCoreApplication::applicationDirPath()+"/plugins");
        if (!dir.exists())
            break;
        dir.setFilter(QDir::Files);
        QFileInfoList list=dir.entryInfoList();
        int file_count=list.count();
        if (file_count<=0)
            break;
        for (QFileInfoList::iterator it=list.begin();it!=list.end();++it) {
            QPluginLoader *qpl=new QPluginLoader(it->absoluteFilePath(),this);
            QObject *plugin=qpl->instance();
            if (!plugin) {
                qpl->deleteLater();
                continue;
            }
            Plugin *ThePlugin=qobject_cast<Plugin*>(plugin);
            if (!ThePlugin) {
                qpl->deleteLater();
                continue;
            }
            if (instances.find(ThePlugin->PluginName)!=instances.end())
                continue;
            instances.insert(ThePlugin->PluginName,ThePlugin);
            if (ThePlugin->Activated)
                ThePlugin->Init(this);
        }
    } while (0);
    pcp=new PluginsControlPanel(instances,this);
}

PluginManager::~PluginManager() {
    pcp->close();
    delete pcp;
    for (QMap<QString,Plugin*>::iterator it=instances.begin();it!=instances.end();++it) {
        if (it.value()->Activated) {
            it.value()->Destroy(this);
        }
    }
}

void PluginManager::ShowPluginsControlPanel() {
    pcp->show();
}
