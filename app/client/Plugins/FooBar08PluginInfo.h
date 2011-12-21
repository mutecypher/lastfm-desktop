#ifndef FOOBAR08_PLUGIN_INFO_H_
#define FOOBAR08_PLUGIN_INFO_H_

#include "IPluginInfo.h"

class FooBar08PluginInfo : public IPluginInfo
{
    Q_OBJECT
public:
    FooBar08PluginInfo( QObject* parent = 0 );

    Version version() const;

    QString name() const;
    Version minVersion() const;
    Version maxVersion() const;
    
    QString pluginPath() const;
    QString displayName() const;
    QString processName() const;

    QString id() const;
    BootstrapType bootstrapType() const;

    QString pluginInstallPath() const;
    QString pluginInstaller() const;
};

#endif //FOOBAR08_PLUGIN_INFO_H_
