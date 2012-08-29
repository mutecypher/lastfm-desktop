#ifndef FOOBAR09_PLUGIN_INFO_H_
#define FOOBAR09_PLUGIN_INFO_H_

#include "IPluginInfo.h"

class FooBar09PluginInfo : public IPluginInfo
{
    Q_OBJECT
public:
    FooBar09PluginInfo( QObject* parent = 0 );

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

#endif //FOOBAR09_PLUGIN_INFO_H_

