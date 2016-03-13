#ifndef USERDATA_H
#define USERDATA_H

#include <QString>

class UserData {
public:
    UserData(const QString &IP,unsigned short Port);
    QString IP;
    unsigned short Port;
    bool Cloak;
};

#endif // USERDATA_H
