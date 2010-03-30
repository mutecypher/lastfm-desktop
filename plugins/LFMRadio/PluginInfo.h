#ifndef LFM_PLUGIN_INFO_H_
#define LFM_PLUGIN_INFO_H_

#include "../../lib/unicorn/Updater/IPluginInfo.h"
#include "../../lib/DllExportMacro.h"

class UNICORN_DLLEXPORT LFMRadioPluginInfo : public IPluginInfo
{
public:
    std::string name() const { return "Last.fm Radio"; }
    Version minVersion() const { return Version(); }
    Version maxVersion() const { return Version(); }
    
    std::string pluginPath() const { return std::string( "" ); }
    std::string displayName() const { return std::string( "" ); }

    std::string id() const { return "ass"; }
    BootstrapType bootstrapType() const { return NoBootstrap; }

    bool isPlatformSupported() const
    {
        return true;
    }

    bool isAppInstalled() const { return true; }
    bool isInstalled() const { return true; }
};

#endif //LFM_PLUGIN_INFO_H_

