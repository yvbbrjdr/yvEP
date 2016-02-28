#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QVariantMap>
#include <QByteArray>
#include <QHostInfo>
#include <QChar>
#include "yvep.h"
#include "mainwindow.h"
#include "serverwindow.h"

namespace Ui {
    class LoginDialog;
}

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent=0);
    ~LoginDialog();
private:
    Ui::LoginDialog *ui;
    yvEP *protocol;
    void SaveConfig();
private slots:
    void LoginPressed();
    void ServerPressed();
    void RecvData(const QString&,unsigned short,const QByteArray &Data);
};

#endif // LOGINDIALOG_H
