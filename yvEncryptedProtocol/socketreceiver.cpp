#include "socketreceiver.h"

SocketReceiver::SocketReceiver(SocketBuffer *buf,QObject *parent):QThread(parent),buf(buf),stop(false) {}

void SocketReceiver::run() {
    while (!stop) {
        buf->mutex.lock();
        int e=buf->RecvBuf.indexOf('}');
        bool quote=0;
        for (int i=0;i<e;++i)
            if (buf->RecvBuf[i]=='\"')
                quote=!quote;
        if (e==-1||quote) {
            buf->mutex.unlock();
            sleep(1);
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
