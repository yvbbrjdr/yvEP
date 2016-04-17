#ifndef PLUGIN_H
#define PLUGIN_H

class PluginManager;

class Plugin {
public:
    virtual void Init(PluginManager *manager);
    virtual void Destroy();
};

#include "pluginmanager.h"

Q_DECLARE_INTERFACE(Plugin,"tk.yvbbrjdr.yvEncryptedChat.Plugin/1.0")

#endif // PLUGIN_H
