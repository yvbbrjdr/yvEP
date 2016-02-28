#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(yvEP *protocol,const QString &ServerIP,unsigned short ServerPort,const QString &Nickname,QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow),protocol(protocol),ServerIP(ServerIP),ServerPort(ServerPort),Nickname(Nickname) {
    ui->setupUi(this);
    listmodel=new QStringListModel(this);
    refreshtimer=new QTimer(this);
    DownLabel=new QLabel;
    DownLabel->setMinimumSize(DownLabel->sizeHint());
    DownLabel->setAlignment(Qt::AlignHCenter);
    ui->statusBar->addWidget(DownLabel);
    ui->statusBar->setStyleSheet("QStatusBar::item{border: 0px}");
    connect(protocol,SIGNAL(RecvData(QString,unsigned short,QByteArray)),this,SLOT(RecvData(QString,unsigned short,QByteArray)));
    connect(ui->Message,SIGNAL(returnPressed()),this,SLOT(SendMessage()));
    connect(ui->RefreshButton,SIGNAL(clicked(bool)),this,SLOT(Refresh()));
    connect(refreshtimer,SIGNAL(timeout()),this,SLOT(Refresh()));
    connect(ui->ClientList,SIGNAL(clicked(QModelIndex)),this,SLOT(Touch(QModelIndex)));
    Refresh();
    refreshtimer->start(30000);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::RecvData(const QString&,unsigned short,const QByteArray &Data) {
    if (Data.left(2)=="li") {
        listmodel->setStringList(QString(Data.mid(3)).split('\n'));
        ui->ClientList->setModel(listmodel);
    } else if (Data.left(2)=="t1") {
        QStringList qsl=QString(Data.mid(2)).split(":");
        RemoteIP=qsl.at(0);
        RemotePort=qsl.at(1).toInt();
        RemoteNickname=qsl.at(2);
        ui->Message->setEnabled(true);
        ui->Message->setFocus();
        if (DownLabel->text().left(DownLabel->text().length()-20)==RemoteNickname)
            DownLabel->setText("");
    } else if (Data.left(2)=="t2") {
        QStringList qsl=QString(Data.mid(2)).split(":");
        protocol->ConnectTo(qsl.at(0),qsl.at(1).toInt());
    } else if (Data.left(2)=="t3") {
        Refresh();
        ui->Message->setEnabled(false);
    } else if (Data[0]=='m') {
        QString n=Data.mid(1,Data.indexOf(':')-1);
        History[n]+=Data.mid(Data.indexOf(':')+1);
        if (n==RemoteNickname) {
            ui->History->setText(History[n]);
            CursorDown();
        } else {
            DownLabel->setText(n+" sent you a message.");
        }
    }
}

void MainWindow::SendMessage() {
    if (ui->Message->text()=="")
        return;
    QString Message;
    Message=QTime::currentTime().toString("hh:mm:ss")+' '+Nickname+'\n'+ui->Message->text()+"\n\n";
    History[RemoteNickname]+=Message;
    ui->History->setText(History[RemoteNickname]);
    CursorDown();
    ui->Message->setText("");
    protocol->ConnectAndSend(RemoteIP,RemotePort,('m'+Nickname+':'+Message).toUtf8());
}

void MainWindow::Refresh() {
    protocol->ConnectAndSend(ServerIP,ServerPort,"li");
}

void MainWindow::closeEvent(QCloseEvent*) {
    protocol->ConnectAndSend(ServerIP,ServerPort,("l3"+Nickname).toUtf8());
}

void MainWindow::Touch(const QModelIndex &index) {
    ui->History->setText(History[index.data().toString()]);
    protocol->ConnectAndSend(ServerIP,ServerPort,("t0"+index.data().toString()).toUtf8());
}

void MainWindow::CursorDown() {
    QTextCursor qtc(ui->History->textCursor());
    qtc.movePosition(QTextCursor::End);
    ui->History->setTextCursor(qtc);
}
