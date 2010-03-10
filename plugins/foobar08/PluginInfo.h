#ifndef FOOBAR08_PLUGIN_INFO_H_
#define FOOBAR08_PLUGIN_INFO_H_

#include "../../lib/unicorn/Updater/IPluginInfo.h"

class FooBar08PluginInfo : public IPluginInfo
{
public:
    std::string name() const { return "Foobar 2000 0.9"; }
    Version minVersion() const { return Version( 0, 9 ); }
    Version maxVersion() const { return Version( 0, 9, 3, 9999 ); }
    
    std::string pluginPath() const { return std::string( "components" ); }
    std::string displayName() const { return std::string( "foobar2000" ); }

    std::string id() const { return std::string( "foo2" ); }
    BootstrapType bootstrapType() const { return NoBootstrap; }

    bool isPlatformSupported() const
    {
        #ifdef WIN32
            return true;
        #endif
        return false;
    }
};

#endif //FOOBAR08_PLUGIN_INFO_H_

