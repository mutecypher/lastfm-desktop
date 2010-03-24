#ifndef WMP_PLUGIN_INFO_H_
#define WMP_PLUGIN_INFO_H_

#include "../../lib/unicorn/Updater/IPluginInfo.h"
#include "../../lib/DllExportMacro.h"

class UNICORN_DLLEXPORT WmpPluginInfo : public IPluginInfo
{
public:
    std::string name() const { return "Windows Media Player"; }
    Version minVersion() const { return Version( 9 ); }
    Version maxVersion() const { return Version(); }
    
    std::string pluginPath() const { return std::string( "Plugins" ); }
    std::string displayName() const { return std::string( "MPlayer2" ); }

    std::string id() const { return "wmp"; }
    BootstrapType bootstrapType() const { return PluginBootstrap; }
    bool isPlatformSupported() const
    {
        #ifdef WIN32
            return true;
        #endif
        return false;
    }
};

#endif //WMP_PLUGIN_INFO_H_

