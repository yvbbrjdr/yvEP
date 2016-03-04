#include "serverwindow.h"
#include "ui_serverwindow.h"

ServerWindow::ServerWindow(yvEP *protocol,QWidget *parent):QMainWindow(parent),ui(new Ui::ServerWindow),protocol(protocol) {
    ui->setupUi(this);
    listmodel=new QStringListModel(this);
    connect(protocol,SIGNAL(RecvData(QString,unsigned short,QByteArray)),this,SLOT(RecvData(QString,unsigned short,QByteArray)));
    connect(ui->ClientList,SIGNAL(clicked(QModelIndex)),this,SLOT(RemoveClient(QModelIndex)));
}

ServerWindow::~ServerWindow() {
    delete ui;
}

void ServerWindow::RecvData(const QString &IP,unsigned short Port,const QByteArray &Data) {
    if (Data.left(2)=="l0") {
        if (Clients.find(Data.mid(2))==Clients.end()) {
            Clients.insert(Data.mid(2),QPair<QString,unsigned short>(IP,Port));
            protocol->ConnectAndSend(IP,Port,"l1");
            UpdateList();
        } else
            protocol->ConnectAndSend(IP,Port,"l2");
    } else if (Data.left(2)=="li") {
        QString s("li\n");
        int count=0;
        for (QMap<QString,QPair<QString,unsigned short> >::iterator it=Clients.begin();it!=Clients.end();++it) {
            s+=it.key()+'\n';
            ++count;
            if (count%20==0) {
                protocol->ConnectAndSend(IP,Port,s.left(s.length()-1).toUtf8());
                s="li\n";
            }
        }
        protocol->ConnectAndSend(IP,Port,s.left(s.length()-1).toUtf8());
    } else if (Data.left(2)=="l3") {
        Clients.remove(QString(Data.mid(2)));
        UpdateList();
    } else if (Data.left(2)=="t0") {
        if (Clients.find(Data.mid(2))==Clients.end())
            protocol->ConnectAndSend(IP,Port,"t3");
        else {
            QPair<QString,unsigned short>ipp=Clients.find(Data.mid(2)).value();
            protocol->ConnectAndSend(IP,Port,("t1"+ipp.first+':'+QString::number(ipp.second)+':'+Data.mid(2)).toUtf8());
            protocol->ConnectAndSend(ipp.first,ipp.second,("t2"+IP+':'+QString::number(Port)).toUtf8());
        }
    }
}

void ServerWindow::UpdateList() {
    QStringList qsl;
    for (QMap<QString,QPair<QString,unsigned short> >::iterator it=Clients.begin();it!=Clients.end();++it)
        qsl.push_back(it.key());
    listmodel->setStringList(qsl);
    ui->ClientList->setModel(listmodel);
}

void ServerWindow::RemoveClient(const QModelIndex &index) {
    Clients.remove(index.data().toString());
    UpdateList();
}
