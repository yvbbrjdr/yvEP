#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QMainWindow>
#include <yvep.h>
#include <QThread>
#include <QMap>
#include <QPair>
#include <QStringList>
#include <QStringListModel>

namespace Ui {
    class ServerWindow;
}

class ServerWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit ServerWindow(yvEP* protocol,QWidget *parent=0);
    ~ServerWindow();
private:
    Ui::ServerWindow *ui;
    yvEP *protocol;
    QMap<QString,QPair<QString,unsigned short> >Clients;
    QStringListModel *listmodel;
    void UpdateList();
private slots:
    void RecvData(const QString &IP,unsigned short Port,const QByteArray &Data);
    void RemoveClient(const QModelIndex &index);
};

#endif // SERVERWINDOW_H
