#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QObject>
#include <QtScript/QScriptEngine>
#include "../plugin.h"

class Calculator : public QObject,public Plugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Calculator")
    Q_INTERFACES(Plugin)
public:
    void Init(PluginManager *manager);
    QScriptEngine qse;
signals:
    void SendMsg(const QString &Nickname,const QString &Content);
private slots:
    void RecvMsg(const QString &Nickname,const QString &Content);
};

#endif // CALCULATOR_H
