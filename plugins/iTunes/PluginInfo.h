#ifndef ITUNES_PLUGIN_INFO_H_
#define ITUNES_PLUGIN_INFO_H_

#include "../../lib/unicorn/Updater/IPluginInfo.h"

class ITunesPluginInfo : public IPluginInfo
{
public:
    std::string name() const { return "iTunes"; }
    Version minVersion() const { return Version(); }
    Version maxVersion() const { return Version(); }
    
    std::string pluginPath() const { return std::string( "Plug-Ins" ); }
    std::string displayName() const { return std::string( "iTunes" ); }

    std::string id() const { return "itw"; }
    BootstrapType bootstrapType() const { return ClientBootstrap; }

    bool isPlatformSupported() const
    {
        #if defined (WIN32) || defined (__APPLE__)
            return true;
        #endif
        return false;
    }
};

#endif //ITUNES_PLUGIN_INFO_H_

