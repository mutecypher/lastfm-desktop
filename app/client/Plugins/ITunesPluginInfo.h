#ifndef ITUNES_PLUGIN_INFO_H_
#define ITUNES_PLUGIN_INFO_H_

#include "IPluginInfo.h"

class ITunesPluginInfo : public IPluginInfo
{
    Q_OBJECT
public:
    ITunesPluginInfo( QObject* parent = 0 );

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

#endif //ITUNES_PLUGIN_INFO_H_

