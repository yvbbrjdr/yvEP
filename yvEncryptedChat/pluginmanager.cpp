#include "pluginmanager.h"

PluginManager::PluginManager(QObject *parent) : QObject(parent) {
    QDir dir(QCoreApplication::applicationDirPath()+"/plugins");
    if (!dir.exists())
        return;
    dir.setFilter(QDir::Files);
    QFileInfoList list=dir.entryInfoList();
    int file_count=list.count();
    if (file_count<=0)
        return;
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
        instances.push_back(ThePlugin);
        ThePlugin->Init(this);
    }
}

PluginManager::~PluginManager() {
    for (int i=0;i<instances.size();++i) {
        instances[i]->Destroy();
    }
}
