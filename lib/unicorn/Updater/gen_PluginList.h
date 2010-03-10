#ifndef GEN_PLUGIN_LIST_H_
#define GEN_PLUGIN_LIST_H_

#include <QList>
#include "IPluginInfo.h"

#include "../../../plugins/foobar08/PluginInfo.h"
#include "../../../plugins/foobar09/PluginInfo.h"
#include "../../../plugins/iTunes/PluginInfo.h"
#include "../../../plugins/winamp/PluginInfo.h"
#include "../../../plugins/wmp/PluginInfo.h"
class GenPluginList : public QList<IPluginInfo*>
{
public:
    GenPluginList()
    { 

        *this << (new FooBar08PluginInfo());
        *this << (new FooBar09PluginInfo());
        *this << (new ITunesPluginInfo());
        *this << (new WinampPluginInfo());
        *this << (new WmpPluginInfo());

    }
};

#endif //GEN_PLUGIN_LIST_H_
