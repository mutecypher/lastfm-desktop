#ifndef PLUGIN_LIST_H_
#define PLUGIN_LIST_H_

#include <QObject>

#include "WinampPluginInfo.h"
#include "WmpPluginInfo.h"
#include "ITunesPluginInfo.h"
#include "Foobar09PluginInfo.h"

class PluginList : public QObject
{
    Q_OBJECT
public:
    PluginList( QObject* parent = 0 ) : QObject( parent )
    {
        m_plugins << (new ITunesPluginInfo( this ));
        m_plugins << (new WmpPluginInfo( this ));
        m_plugins << (new WinampPluginInfo( this ));
        m_plugins << (new FooBar09PluginInfo( this ));
    }

    QList<IPluginInfo*> availablePlugins() const;
    QList<IPluginInfo*> installedPlugins() const;
    QList<IPluginInfo*> bootstrappablePlugins() const;
    QList<IPluginInfo*> installList() const;

    QList<IPluginInfo*> updatedList() const;
    QList<IPluginInfo*> supportedList() const;
    QString availableDescription() const;

    IPluginInfo* pluginById( const QString& id ) const;

private:
    QList<IPluginInfo*> m_plugins;
};

#endif //PLUGIN_LIST_H_
