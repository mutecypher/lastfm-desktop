#ifndef ITUNES_PLUGIN_INFO_H_
#define ITUNES_PLUGIN_INFO_H_

#include "IPluginInfo.h"

#include <lib/DllExportMacro.h>

namespace unicorn
{

class UNICORN_DLLEXPORT ITunesPluginInfo : public IPluginInfo
{
    Q_OBJECT
public:
    ITunesPluginInfo( QObject* parent = 0 );

    Version version() const;
    QString name() const;

    QString displayName() const;
    QString processName() const;

    QString id() const;
    BootstrapType bootstrapType() const;

    bool isAppInstalled() const;
    QString pluginInstallPath() const;
    QString pluginInstaller() const;

#ifdef Q_OS_MAC
    // the iTunes plugin is always installed on mac
    bool isInstalled() const { return true; }
#endif
};

}

#endif //ITUNES_PLUGIN_INFO_H_

