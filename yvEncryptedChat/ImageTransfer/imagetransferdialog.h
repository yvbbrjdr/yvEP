#ifndef IMAGETRANSFERDIALOG_H
#define IMAGETRANSFERDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QBuffer>
#include "../pluginmanager.h"
#include "imageshowdialog.h"

namespace Ui {
    class ImageTransferDialog;
}

class ImageTransferDialog : public QDialog {
    Q_OBJECT
public:
    explicit ImageTransferDialog(PluginManager *manager);
    ~ImageTransferDialog();
private:
    Ui::ImageTransferDialog *ui;
signals:
    void SendMsg(const QString &Nickname,const QString &Content);
private slots:
    void RecvMsg(const QString &Nickname,const QString &Content);
    void Transfer();
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // IMAGETRANSFERDIALOG_H
