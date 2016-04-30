/*
This file is part of yvEncryptedChat
yvEncryptedChat is an IM program that provides secure chats.
Copyright (C) 2016  yvbbrjdr

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

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
