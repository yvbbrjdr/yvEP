#include "calculator.h"

void Calculator::Init(PluginManager *manager) {
    connect(manager,SIGNAL(RecvMsg(QString,QString)),this,SLOT(RecvMsg(QString,QString)));
    connect(this,SIGNAL(SendMsg(QString,QString)),manager,SIGNAL(SendMsg(QString,QString)));
}

void Calculator::RecvMsg(const QString &Nickname,const QString &Content) {
    emit SendMsg(Nickname,qse.evaluate(Content).toString());
}
