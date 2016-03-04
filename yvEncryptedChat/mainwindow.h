#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QMessageBox>
#include <yvep.h>
#include <QStringList>
#include <QStringListModel>
#include <QTimer>
#include <QMap>
#include <QTextCursor>
#include <QLabel>

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
    QString RemoteIP;
    unsigned short RemotePort;
    QString RemoteNickname;
    QStringList Clients;
    QStringListModel *listmodel;
    QTimer *refreshtimer;
    QMap<QString,QString>History;
    void CursorDown();
    QLabel *DownLabel;
    void UpdateClients();
private slots:
    void RecvData(const QString&,unsigned short,const QByteArray &Data);
    void SendMessage();
    void Refresh();
    void Touch(const QModelIndex &index);
protected:
    void closeEvent(QCloseEvent*);
};

#endif // MAINWINDOW_H
