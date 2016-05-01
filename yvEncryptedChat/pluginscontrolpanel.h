#ifndef PLUGINSCONTROLPANEL_H
#define PLUGINSCONTROLPANEL_H

#include <QDialog>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QStringListModel>

class Plugin;
class PluginManager;

namespace Ui {
    class PluginsControlPanel;
}

class PluginsControlPanel : public QDialog {
    Q_OBJECT
public:
    explicit PluginsControlPanel(const QMap<QString,Plugin*>&instances,PluginManager *manager,QWidget *parent = 0);
    ~PluginsControlPanel();
private:
    Ui::PluginsControlPanel *ui;
    QMap<QString,Plugin*>instances;
    QStringListModel *qslm;
    PluginManager *manager;
private slots:
    void Activate();
    void Deactivate();
    void Function();
    void ListClick(const QModelIndex &qmi);
};

#include "plugin.h"
#include "pluginmanager.h"

#endif // PLUGINSCONTROLPANEL_H
