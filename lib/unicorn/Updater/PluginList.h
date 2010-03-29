#ifndef PLUGIN_LIST_H_
#define PLUGIN_LIST_H_

#include "gen_PluginList.h"
#include "lib/DllExportMacro.h"

class UNICORN_DLLEXPORT PluginList : public GenPluginList
{
public:
    QList<IPluginInfo*> availablePlugins() const;
    QList<IPluginInfo*> installedPlugins() const;
    QList<IPluginInfo*> bootstrappablePlugins() const;

    PluginList supportedList() const;
};

#endif //PLUGIN_LIST_H_
