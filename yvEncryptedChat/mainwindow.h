#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QMessageBox>
#include "../yvEncryptedProtocol/yvep.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    yvEP *yvep;
private slots:
    void RecvData(const QString &IP,unsigned short Port,const QByteArray &Data);
    void on_pushButton_clicked();
};

#endif // MAINWINDOW_H
