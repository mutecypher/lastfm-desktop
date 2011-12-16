#ifndef PLUGIN_LIST_H_
#define PLUGIN_LIST_H_

#include "gen_PluginList.h"

class PluginList : public GenPluginList
{
    Q_OBJECT
public:
    PluginList( QObject* parent = 0 ) : GenPluginList( parent ) {}

    QList<IPluginInfo*> availablePlugins() const;
    QList<IPluginInfo*> installedPlugins() const;
    QList<IPluginInfo*> bootstrappablePlugins() const;

    QList<IPluginInfo*> supportedList() const;
    QString availableDescription() const;

    IPluginInfo* pluginById( const QString& id ) const;
};

#endif //PLUGIN_LIST_H_
