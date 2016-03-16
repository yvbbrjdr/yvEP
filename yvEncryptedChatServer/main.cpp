#include <QCoreApplication>
#include "mainthread.h"
#include <QString>
#include <cstdio>
#include <cstdlib>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    if (argc>2) {
        printf("Usage: %s [Port]\n",argv[0]);
        exit(0);
    }
    unsigned short Port=2428;
    if (argc==2)
        Port=QString(argv[1]).toShort();
    printf("Welcome to yvEncryptedChatServer\n\nServer will be listening at Port %d\nGenerating RSA Key. . . ",Port);
    fflush(stdout);
    MainThread thread(Port);
    thread.start();
    puts("Done!\n\nEnter nickname to logout");
    return a.exec();
}
