#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QThread>
#include <yvep.h>
#include <QMap>

class ServerThread : public QThread {
    Q_OBJECT
public:
    ServerThread(unsigned short Port);
private:
    yvEP *protocol;
    QMap<QString,QPair<QString,unsigned short> >Clients;
private slots:
    void RecvData(const QString &IP,unsigned short Port,const QByteArray &Data);
    void RemoveClient(const QString &Nickname);
};

#endif // SERVERTHREAD_H
