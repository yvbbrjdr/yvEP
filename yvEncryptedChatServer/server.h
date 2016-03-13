#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <yvep.h>
#include "userdata.h"

class Server : public QObject {
    Q_OBJECT
public:
    Server(unsigned short Port);
private:
    yvEP *protocol;
    QMap<QString,UserData>Clients;
private slots:
    void RecvData(const QString &IP,unsigned short Port,const QByteArray &Data);
    void RemoveClient(const QString &Nickname);
};

#endif // SERVER_H
