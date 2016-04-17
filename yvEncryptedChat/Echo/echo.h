#ifndef ECHO_H
#define ECHO_H

#include "../plugin.h"

class Echo : public QObject,public Plugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Echo")
    Q_INTERFACES(Plugin)
public:
    void Init(PluginManager *manager);
};

#endif // ECHO_H
