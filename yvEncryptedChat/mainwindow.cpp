#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow) {
    ui->setupUi(this);
    yvep=new yvEP(8080,this);
    connect(yvep,SIGNAL(RecvData(QString,unsigned short,QByteArray)),this,SLOT(RecvData(QString,unsigned short,QByteArray)));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::RecvData(const QString &IP,unsigned short Port,const QByteArray &Data) {
    QMessageBox::information(this,"data",QString(Data)+'\n'+IP+':'+QString::number(Port));
}

void MainWindow::on_pushButton_clicked() {
    if (ui->lineEdit->text()!=yvep->CurRemoteIP()) {
        yvep->ConnectTo(ui->lineEdit->text(),8080);
        return;
    }
    yvep->SendData("Hello, yv! \n");
}
