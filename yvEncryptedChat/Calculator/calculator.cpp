#include "calculator.h"

void Calculator::Init(PluginManager *manager) {
    connect(manager,SIGNAL(RecvMsg(QString,QString)),this,SLOT(RecvMsg(QString,QString)));
    connect(this,SIGNAL(SendMsg(QString,QString)),manager,SIGNAL(SendMsg(QString,QString)));
}

void Calculator::RecvMsg(const QString &Nickname,const QString &Content) {
    if (Nickname=="Broadcast")
        return;
    for (int i=0;i<Content.length();++i) {
        if (Content[i].isLetter()) {
            emit SendMsg(Nickname,"Letters are forbidden in the expression");
            return;
        }
    }
    emit SendMsg(Nickname,qse.evaluate(Content).toString());
}
