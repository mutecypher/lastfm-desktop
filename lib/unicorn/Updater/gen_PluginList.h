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

        *this << (new UNICORN_DLLEXPORT FooBar08PluginInfo());
        *this << (new UNICORN_DLLEXPORT FooBar09PluginInfo());
        *this << (new UNICORN_DLLEXPORT ITunesPluginInfo());
        *this << (new UNICORN_DLLEXPORT WinampPluginInfo());
        *this << (new UNICORN_DLLEXPORT WmpPluginInfo());

    }
};

#endif //GEN_PLUGIN_LIST_H_
