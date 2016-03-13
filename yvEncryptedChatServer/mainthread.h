#ifndef MAINTHREAD_H
#define MAINTHREAD_H

#include <QThread>
#include <cstdio>
#include "server.h"

class MainThread : public QThread {
    Q_OBJECT
public:
    MainThread(unsigned short Port);
protected:
    void run();
private:
    Server *server;
signals:
    void RemoveClient(const QString &Nickname);
};

#endif // MAINTHREAD_H
