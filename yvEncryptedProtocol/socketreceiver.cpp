#include "socketreceiver.h"

SocketReceiver::SocketReceiver(SocketBuffer *buf,QObject *parent):QThread(parent),buf(buf),stop(false) {}

void SocketReceiver::run() {
    while (!stop) {
        buf->mutex.lock();
        int e=buf->RecvBuf.indexOf('}');
        if (e==-1) {
            buf->mutex.unlock();
            sleep(0);
            continue;
        }
        emit RecvData(QJsonDocument::fromJson(buf->RecvBuf.left(e+1)).toVariant().toMap());
        buf->RecvBuf=buf->RecvBuf.mid(e+1);
        buf->mutex.unlock();
    }
}

void SocketReceiver::Stop() {
    stop=true;
}
