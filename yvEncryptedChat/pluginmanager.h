#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QPluginLoader>
#include <QString>
#include <QVector>
#include <QDir>
#include <QCoreApplication>
#include "plugin.h"

class PluginManager : public QObject {
    Q_OBJECT
private:
    QVector<Plugin*>instances;
public:
    explicit PluginManager(QObject *parent = 0);
    ~PluginManager();
signals:
    void RecvMsg(const QString &Nickname,const QString &Content);
    void SendMsg(const QString &Nickname,const QString &Content);
};

#endif // PLUGINMANAGER_H
