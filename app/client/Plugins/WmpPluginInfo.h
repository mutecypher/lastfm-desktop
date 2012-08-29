#ifndef WMP_PLUGIN_INFO_H_
#define WMP_PLUGIN_INFO_H_

#include "../Plugins/IPluginInfo.h"

class WmpPluginInfo : public IPluginInfo
{
    Q_OBJECT
public:
    WmpPluginInfo( QObject* parent = 0 );

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

#endif //WMP_PLUGIN_INFO_H_

