#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../../yvEP.hpp"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <queue>
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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    Turn(server.c_str(),port);
    Send(("r3"+nickname).c_str());
    Close();
    delete ui;
}
