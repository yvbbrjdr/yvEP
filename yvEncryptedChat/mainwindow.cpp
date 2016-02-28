#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(yvEP *protocol,const QString &ServerIP,unsigned short ServerPort,const QString &Nickname,QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow),protocol(protocol),ServerIP(ServerIP),ServerPort(ServerPort),Nickname(Nickname) {
    ui->setupUi(this);
    listmodel=new QStringListModel(this);
    connect(protocol,SIGNAL(RecvData(QString,unsigned short,QByteArray)),this,SLOT(RecvData(QString,unsigned short,QByteArray)));
    connect(ui->Message,SIGNAL(returnPressed()),this,SLOT(SendMessage()));
    connect(ui->RefreshButton,SIGNAL(clicked(bool)),this,SLOT(Refresh()));
    Refresh();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::RecvData(const QString &IP,unsigned short Port,const QByteArray &Data) {
    if (Data.left(2)=="li") {
        listmodel->setStringList(QString(Data.mid(3)).split('\n'));
        ui->ClientList->setModel(listmodel);
    }
}

void MainWindow::SendMessage() {
    ui->History->append(ui->Message->text());
    ui->Message->setText("");
}

void MainWindow::Refresh() {
    protocol->ConnectAndSend(ServerIP,ServerPort,"li");
}
