#include "PluginList.h"
#include <QStringList>

QList<IPluginInfo*> 
PluginList::availablePlugins() const
{
    QList<IPluginInfo*> ret;
    foreach( IPluginInfo* plugin, m_plugins ) {
        if( plugin->isInstalled() ||
            !plugin->isPlatformSupported() ) continue;
        if( plugin->isAppInstalled() )
            ret << plugin;
    }
    return ret;
}

QList<IPluginInfo*> 
PluginList::installedPlugins() const
{
    QList<IPluginInfo*> ret;
    foreach( IPluginInfo* plugin, m_plugins ) {
        if( plugin->isInstalled())
            ret << plugin;
    }
    return ret;
}

QList<IPluginInfo*>
PluginList::bootstrappablePlugins() const
{
    QList<IPluginInfo*> ret;
    foreach( IPluginInfo* plugin, installedPlugins() ) {
        if( plugin->canBootstrap()) {
            ret << plugin;
        }
    }
    return ret;
}

QList<IPluginInfo*>
PluginList::supportedList() const
{
    QList<IPluginInfo*> ret;
    foreach( IPluginInfo* i, m_plugins ) {
        if( i->isPlatformSupported()) {
            ret << i;
        }
    }
    return ret;
}

QString
PluginList::availableDescription() const
{
    QStringList mediaPlayers;
    
    foreach( IPluginInfo* i, supportedList() )
    {
        mediaPlayers << QString::fromStdString( i->name() );
    }

    QString ret = mediaPlayers.takeLast();
    
    if( mediaPlayers.count() > 0 )
        ret = mediaPlayers.join( ", " ) + " or " + ret;

    return ret;
}

IPluginInfo* 
PluginList::pluginById( const QString& id ) const
{
    foreach( IPluginInfo* plugin, m_plugins )
    {
        if( !plugin->id().compare( id.toStdString()))
            return plugin;
    }

    return NULL;
}
