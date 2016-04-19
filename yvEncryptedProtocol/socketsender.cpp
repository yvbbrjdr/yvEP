#include "socketsender.h"

SocketSender::SocketSender(SocketBuffer *buf,QObject *parent):QThread(parent),buf(buf),stop(false),count(0),count1(0),count2(0) {}

void SocketSender::run() {
    while (!stop) {
        buf->mutex.lock();
        if (buf->PublicKey.size()==0) {
            ++count;
            if (count==100000)
                emit Reset();
            buf->mutex.unlock();
            sleep(1);
            continue;
        }
        if (buf->SendNonce!=buf->ReplyNonce) {
            ++count1;
            if (count1==10000) {
                ++count2;
                if (count2==10) {
                    buf->mutex.unlock();
                    emit Reset();
                    break;
                }
                count1=0;
                emit SendRaw(buf->LastSend);
            }
            buf->mutex.unlock();
            sleep(1);
            continue;
        }
        if (buf->SendBuf.size()==0) {
            buf->mutex.unlock();
            sleep(1);
            continue;
        }
        count1=count2=0;
        sodium_increment((unsigned char*)buf->SendNonce.data(),buf->SendNonce.length());
        buf->LastSend=buf->SendBuf.left(512);
        buf->SendBuf=buf->SendBuf.mid(512);
        emit SendRaw(buf->LastSend);
        buf->mutex.unlock();
    }
}

void SocketSender::Stop() {
    stop=true;
}
