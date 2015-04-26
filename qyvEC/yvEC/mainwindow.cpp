#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>
#include <QTimer>
#include "../../yvEP.hpp"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <queue>
#include <map>
#include <vector>
#include <cstdlib>
#include <unistd.h>
using namespace yvEP;
using namespace std;

queue<string>que;
unsigned char loggedin;
char tstr[300],ip2[20];
string server="yvec.yvbb.tk",nickname;
in_port_t port=2428,port2;
map<string,string>jl;
Ui::MainWindow *u;

void Recv(const char*,in_port_t,size_t,const char *Content) {que.push(Content);}

void *processqueue(void*) {
    while (1)
        if (que.empty())
            usleep(10000);
        else {
            const char *Content=que.front().c_str();
            que.pop();
            if (*Content=='r')
                loggedin=Content[1]-'0';
            else if (*Content=='l') {
                int len=strlen(Content);
                QString t;
                u->listWidget->clear();
                for (int i=2;i<len;++i)
                    if (Content[i]=='\n'){
                        u->listWidget->addItem(t);
                        t="";
                    } else
                        t+=Content[i];
            } else if (*Content=='c') {
                if (Content[1]=='1') {
                    strcpy(tstr,Content+2);
                    *strchr(tstr,':')=0;
                    udp->Send(tstr,atoi(&tstr[strlen(tstr)+1]),5,"test");
                } else if (Content[1]=='2') {
                    strcpy(tstr,Content+2);
                    *strchr(tstr,':')=0;
                    strcpy(ip2,tstr);
                    port2=atoi(&tstr[strlen(tstr)+1]);
                    udp->Send(ip2,port2,5,"test");
                    u->pushButton_2->setEnabled(1);
                    u->lineEdit->setEnabled(1);
                } else if (Content[1]=='3') {
                    Turn(server.c_str(),port);
                    Send("l0");
                }
            } else if (*Content=='m') {
                strcpy(tstr,Content+1);
                *strchr(tstr,char(-35))=0;
                time_t t=time(0);
                tm *current_time = localtime(&t);
                char tt[500];
                sprintf(tt,"%d:%d:%d %s\n%s\n\n",current_time->tm_hour,current_time->tm_min,current_time->tm_sec,tstr,&tstr[strlen(tstr)+1]);
                jl[ip2]+=tt;
            }
        }
    return NULL;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    u=ui;
    pthread_t tid;
    pthread_create(&tid,0,processqueue,0);
    timer=new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(update()));
    Init(2429,Recv,NULL,NULL);
    FILE *sf=fopen("Server.yvEC","r");
    if (sf) {
        fgets(tstr,1000,sf);
        tstr[strlen(tstr)-1]=0;
        fclose(sf);
        *strchr(tstr,':')=0;
        server=tstr;
        port=atoi(&tstr[strlen(tstr)+1]);
    } else {
        sf=fopen("Server.yvEC","w");
        fprintf(sf,"%s:%d\n",server.c_str(),port);
        fclose(sf);
    }
    sprintf(tstr,"%s:%d",server.c_str(),port);
    QInputDialog *inp=new QInputDialog;
    do {
        QString t=inp->getText(this,"Input Server"," Please Input Server Name And Port: ",QLineEdit::Normal,tstr,0,0,Qt::ImhNone);
        if (t=="")
            exit(0);
        QByteArray ba=t.toLatin1();
        strcpy(tstr,ba.data());
        sf=fopen("Server.yvEC","w");
        fputs(tstr,sf);
        fputc('\n',sf);
        fclose(sf);
        *strchr(tstr,':')=0;
        server=tstr;
        port=atoi(&tstr[strlen(tstr)+1]);
        tstr[strlen(tstr)]=':';
    } while (!Turn(server.c_str(),port));
    while (1) {
        QString t=inp->getText(this,"Input Nickname","Please Input Your Nickname: ",QLineEdit::Normal,"",0,0,Qt::ImhNone);
        if (t=="")
            exit(0);
        nickname=t.toStdString();
        Send(("r0"+nickname).c_str());
        while (!loggedin);
        if (loggedin==1)
            break;
        else
            loggedin=0;
    }
    timer->start(100);
    on_pushButton_clicked();
}

MainWindow::~MainWindow()
{
    Turn(server.c_str(),port);
    Send(("r3"+nickname).c_str());
    Close();
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    if (ui->lineEdit->text().toStdString()!="") {
        strcpy(tstr,ui->lineEdit->text().toStdString().c_str());
        Turn(ip2,port2);
        Send(('m'+nickname+char(-35)+tstr).c_str());
        time_t t=time(0);
        tm *current_time = localtime(&t);
        char tt[1000];
        sprintf(tt,"%d:%d:%d %s\n%s\n\n",current_time->tm_hour,current_time->tm_min,current_time->tm_sec,nickname.c_str(),tstr);
        jl[ip2]+=tt;
        ui->lineEdit->clear();
    }
    ui->lineEdit->setFocus();
}

void MainWindow::on_pushButton_clicked()
{
    Turn(server.c_str(),port);
    Send("l0");
}

void MainWindow::on_listWidget_clicked(const QModelIndex &index)
{
    Turn(server.c_str(),port);
    Send(("c0"+index.data(Qt::DisplayRole).toString().toStdString()).c_str());
}

void MainWindow::update() {
    if (ui->plainTextEdit->toPlainText().toStdString()!=jl[ip2]) {
        ui->plainTextEdit->clear();
        ui->plainTextEdit->appendPlainText(jl[ip2].c_str());
    }
}

void MainWindow::on_lineEdit_editingFinished() {
    if (ui->lineEdit->text().toStdString()!="")
        on_pushButton_2_clicked();
}
