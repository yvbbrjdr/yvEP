#ifndef IMAGESHOWDIALOG_H
#define IMAGESHOWDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>

namespace Ui {
    class ImageShowDialog;
}

class ImageShowDialog : public QDialog {
    Q_OBJECT
public:
    explicit ImageShowDialog(const QString &Title,const QPixmap &Pixmap,QWidget *parent);
    ~ImageShowDialog();
private:
    Ui::ImageShowDialog *ui;
private slots:
    void SaveImage();
};

#endif // IMAGESHOWDIALOG_H
