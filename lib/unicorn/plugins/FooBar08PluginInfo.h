#ifndef FOOBAR08_PLUGIN_INFO_H_
#define FOOBAR08_PLUGIN_INFO_H_

#include "IPluginInfo.h"

#include <lib/DllExportMacro.h>

namespace unicorn
{

class UNICORN_DLLEXPORT FooBar08PluginInfo : public IPluginInfo
{
    Q_OBJECT
public:
    FooBar08PluginInfo( QObject* parent = 0 );

    Version version() const;
    QString name() const;

    QString displayName() const;
    QString processName() const;

    QString id() const;
    BootstrapType bootstrapType() const;

    bool isAppInstalled() const;
    QString pluginInstallPath() const;
    QString pluginInstaller() const;
};

}

#endif //FOOBAR08_PLUGIN_INFO_H_
