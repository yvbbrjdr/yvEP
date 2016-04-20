#ifndef IMAGETRANSFER_H
#define IMAGETRANSFER_H

#include <QObject>
#include "../plugin.h"
#include "imagetransferdialog.h"

class ImageTransfer : public QObject,public Plugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "ImageTransfer")
    Q_INTERFACES(Plugin)
private:
    ImageTransferDialog *itd;
public:
    void Init(PluginManager *manager);
    void Destroy();
};

#endif // IMAGETRANSFER_H
