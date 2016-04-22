#include "configmanager.h"

QVariantMap ConfigManager::GetConfig(const QString &Filename) {
    QFile file(Filename);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text))
        return QVariantMap();
    QTextStream qts(&file);
    QVariantMap ret(QJsonDocument::fromJson(qts.readAll().toUtf8()).toVariant().toMap());
    file.close();
    return ret;
}

bool ConfigManager::SetConfig(const QString &Filename,const QVariantMap &Config) {
    QFile file(Filename);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text))
        return false;
    QTextStream qts(&file);
    qts<<QJsonDocument::fromVariant(Config).toJson();
    file.close();
    return true;
}
