#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QJsonDocument>
#include <QVariantMap>
#include <QString>
#include <QFile>
#include <QTextStream>

class ConfigManager {
public:
    QVariantMap GetConfig(const QString &Filename);
    bool SetConfig(const QString &Filename,const QVariantMap &Config);
};

#endif // CONFIGMANAGER_H
