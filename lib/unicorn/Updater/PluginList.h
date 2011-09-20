#ifndef PLUGIN_LIST_H_
#define PLUGIN_LIST_H_

#include "gen_PluginList.h"
#include "lib/DllExportMacro.h"

class UNICORN_DLLEXPORT PluginList : public GenPluginList
{
public:
    PluginList(){}
    PluginList( const PluginList& that ) : GenPluginList( that ) {}

    QList<IPluginInfo*> availablePlugins() const;
    QList<IPluginInfo*> installedPlugins() const;
    QList<IPluginInfo*> bootstrappablePlugins() const;

    QList<IPluginInfo*> supportedList() const;
    QString availableDescription() const;

    IPluginInfo* pluginById( const QString& id ) const;
};

#endif //PLUGIN_LIST_H_
