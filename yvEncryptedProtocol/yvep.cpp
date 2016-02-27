#include "yvep.h"

#define KEYLEN (2048)

yvEP::yvEP(unsigned short Port,QObject *parent):QObject(parent) {
    socket=new UdpSocket(Port);
    thread=new QThread(this);
    RemotePort=0;
    LocalRSA=RSA_generate_key(KEYLEN,RSA_F4,NULL,NULL);
    RemoteRSA=RSA_new();
    unsigned char pubkey[KEYLEN];
    unsigned char *tkey=pubkey;
    int keylen=i2d_RSAPublicKey(LocalRSA,&tkey);
    PublicKey=QByteArray((const char*)pubkey,keylen);
    RemoteRSA=NULL;
    connect(socket,SIGNAL(RecvData(QString,unsigned short,QByteArray)),this,SLOT(ProcessData(QString,unsigned short,QByteArray)));
    socket->moveToThread(thread);
    thread->start();
}

QString yvEP::CurRemoteIP() {
    return RemoteIP;
}

unsigned short yvEP::CurRemotePort() {
    return RemotePort;
}

void yvEP::ConnectTo(const QString &IP,unsigned short Port) {
    emit socket->SendData(IP,Port,"0");
}

void yvEP::SendData(const QByteArray &Data) {
    if (RemoteIP=="")
        return;
    unsigned char t[KEYLEN];
    int len=RSA_public_encrypt(Data.length(),(unsigned char*)Data.data(),t,RemoteRSA,RSA_PKCS1_PADDING);
    QByteArray qba("2");
    qba.append((const char*)t,len);
    emit socket->SendData(RemoteIP,RemotePort,qba);
}

void yvEP::ProcessData(const QString &IP,unsigned short Port,const QByteArray &Data) {
    char op=Data.at(0);
    if (op=='0') {
        QByteArray qba("1");
        qba.append(PublicKey);
        emit socket->SendData(IP,Port,qba);
        emit ConnectYou(IP,Port);
    } else if (op=='1') {
        RemoteIP=IP;
        RemotePort=Port;
        unsigned char *tkey=(unsigned char*)(Data.data())+1;
        RSA_free(RemoteRSA);
        RemoteRSA=d2i_RSAPublicKey(NULL,(const unsigned char**)(&tkey),Data.length()-1);
        emit ConnectedTo(IP,Port);
    } else if (op=='2') {
        unsigned char t[KEYLEN>>3];
        int len=RSA_private_decrypt(Data.length()-1,(unsigned char*)(Data.data())+1,t,LocalRSA,RSA_PKCS1_PADDING);
        emit RecvData(IP,Port,QByteArray((const char*)t,len));
    }
}

yvEP::~yvEP() {
    thread->quit();
    thread->wait();
    socket->deleteLater();
    RSA_free(LocalRSA);
    RSA_free(RemoteRSA);
}
