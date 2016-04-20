#include "imageshowdialog.h"
#include "ui_imageshowdialog.h"

ImageShowDialog::ImageShowDialog(const QString &Title,const QPixmap &Pixmap,QWidget *parent):QDialog(parent),ui(new Ui::ImageShowDialog) {
    ui->setupUi(this);
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(SaveImage()));
    setWindowTitle(Title);
    ui->label->setPixmap(Pixmap);
}

ImageShowDialog::~ImageShowDialog() {
    delete ui;
}

void ImageShowDialog::SaveImage() {
    QString Filename=QFileDialog::getSaveFileName(this,"Save the image",QString(),"PNG(*.png)");
    if (Filename=="")
        return;
    if (Filename.right(4).toLower()!=".png")
        Filename+=".png";
    if (!ui->label->pixmap()->save(Filename))
        QMessageBox::critical(this,"Error","Failed to save the image");
}
