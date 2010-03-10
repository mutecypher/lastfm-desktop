#ifndef PLUGIN_LIST_H_
#define PLUGIN_LIST_H_

#include "gen_PluginList.h"
#include "lib/DllExportMacro.h"

class UNICORN_DLLEXPORT PluginList : public GenPluginList
{
public:
    QList<IPluginInfo*> availablePlugins() const
    {
        QList<IPluginInfo*> ret;
        foreach( IPluginInfo* plugin, *this ) {
            if( plugin->isInstalled() ||
                !plugin->isPlatformSupported() ) continue;
            if( plugin->isAppInstalled() )
                ret << plugin;
        }
        return ret;
    }

    QList<IPluginInfo*> installedPlugins() const
    {
        QList<IPluginInfo*> ret;
        foreach( IPluginInfo* plugin, *this ) {
            if( plugin->isInstalled())
                ret << plugin;
        }
        return ret;
    }

    QList<IPluginInfo*> bootstrappablePlugins() const
    {
        QList<IPluginInfo*> ret = installedPlugins();
        foreach( IPluginInfo* plugin, ret ) {
            if( !plugin->canBootstrap())
                ret.removeAll( plugin );
        }
        return ret;
    }
};

#endif //PLUGIN_LIST_H_
