#include <QCoreApplication>
#include "mainthread.h"
#include <QString>
#include <cstdio>
#include <cstdlib>

int main(int argc, char *argv[]) {
    if (argc!=2) {
        printf("Usage: %s <Port>\n",argv[0]);
        exit(0);
    }
    QCoreApplication a(argc, argv);
    MainThread thread(QString(argv[1]).toShort());
    thread.start();
    return a.exec();
}
