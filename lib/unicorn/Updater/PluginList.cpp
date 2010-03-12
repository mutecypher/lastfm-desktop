#include "PluginList.h"

QList<IPluginInfo*> PluginList::availablePlugins() const
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

QList<IPluginInfo*> PluginList::installedPlugins() const
{
    QList<IPluginInfo*> ret;
    foreach( IPluginInfo* plugin, *this ) {
        if( plugin->isInstalled())
            ret << plugin;
    }
    return ret;
}

QList<IPluginInfo*> PluginList::bootstrappablePlugins() const
{
    QList<IPluginInfo*> ret = installedPlugins();
    foreach( IPluginInfo* plugin, ret ) {
        if( !plugin->canBootstrap())
            ret.removeAll( plugin );
    }
    return ret;
}

