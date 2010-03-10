#ifndef WINAMP_PLUGIN_INFO_H_
#define WINAMP_PLUGIN_INFO_H_

#include "../../lib/unicorn/Updater/IPluginInfo.h"

class WinampPluginInfo : public IPluginInfo
{
public:
    std::string name() const { return "Winamp"; }
    Version minVersion() const { return Version(); }
    Version maxVersion() const { return Version(); }
    
    std::string pluginPath() const { return std::string( "plugins" ); }
    std::string displayName() const { return std::string( "MPlayer2" ); }

    std::string id() const { return "wa2"; }
    BootstrapType bootstrapType() const { return PluginBootstrap; }
    bool isPlatformSupported() const
    {
        #ifdef WIN32
            return true;
        #endif
        return false;
    }
};

#endif //WINAMP_PLUGIN_INFO_H_

