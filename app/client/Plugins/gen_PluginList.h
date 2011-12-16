#ifndef GEN_PLUGIN_LIST_H_
#define GEN_PLUGIN_LIST_H_

#include <QList>
#include "IPluginInfo.h"

#include "Foobar09PluginInfo.h"
#include "ITunesPluginInfo.h"
#include "WinampPluginInfo.h"
#include "WmpPluginInfo.h"

class GenPluginList : public QObject
{
    Q_OBJECT
public:
    GenPluginList( QObject* parent = 0 )
    :QObject( parent )
    {
        m_plugins << (new ITunesPluginInfo( this ));
        m_plugins << (new WmpPluginInfo( this ));
        m_plugins << (new WinampPluginInfo( this ));
        m_plugins << (new FooBar09PluginInfo( this ));
    }

protected:
    QList<IPluginInfo*> m_plugins;
};

#endif //GEN_PLUGIN_LIST_H_
