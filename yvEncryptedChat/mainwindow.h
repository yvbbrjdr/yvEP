#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QMessageBox>
#include <yvep.h>
#include <QStringList>
#include <QStringListModel>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(yvEP *protocol,const QString &ServerIP,unsigned short ServerPort,const QString &Nickname,QWidget *parent=0);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    yvEP *protocol;
    QString ServerIP;
    unsigned short ServerPort;
    QString Nickname;
    QStringListModel *listmodel;
private slots:
    void RecvData(const QString &IP,unsigned short Port,const QByteArray &Data);
    void SendMessage();
    void Refresh();
};

#endif // MAINWINDOW_H
