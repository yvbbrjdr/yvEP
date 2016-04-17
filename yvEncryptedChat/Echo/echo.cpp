#include "echo.h"

void Echo::Init(PluginManager *manager) {
    connect(manager,SIGNAL(RecvMsg(QString,QString)),manager,SIGNAL(SendMsg(QString,QString)));
}
