#ifndef MAINTHREAD_H
#define MAINTHREAD_H

#include <QThread>
#include <cstdio>
#include "serverthread.h"

class MainThread : public QThread {
    Q_OBJECT
public:
    MainThread(unsigned short Port);
protected:
    void run();
private:
    ServerThread *server;
signals:
    void RemoveClient(const QString &Nickname);
};

#endif // MAINTHREAD_H
