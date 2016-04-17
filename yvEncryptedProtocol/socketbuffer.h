#ifndef SOCKETBUFFER_H
#define SOCKETBUFFER_H

#include <QByteArray>
#include <QMutex>

class SocketBuffer {
public:
    QByteArray PublicKey,SendBuf,RecvBuf,SendNonce,RecvNonce,ReplyNonce,LastSend;
    QMutex mutex;
};

#endif // SOCKETBUFFER_H
