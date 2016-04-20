#include "imagetransfer.h"

void ImageTransfer::Init(PluginManager *manager) {
    itd=new ImageTransferDialog(manager);
    itd->show();
}

void ImageTransfer::Destroy() {
    itd->close();
    delete itd;
}
