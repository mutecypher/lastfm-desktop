#ifndef FOOBAR09_PLUGIN_INFO_H_
#define FOOBAR09_PLUGIN_INFO_H_

#include "../../lib/unicorn/Updater/IPluginInfo.h"
#include "../../lib/DllExportMacro.h"

class UNICORN_DLLEXPORT FooBar09PluginInfo : public IPluginInfo
{
public:
    std::string name() const { return "Foobar 2000 0.9.4"; }
    Version minVersion() const { return Version( 0, 9, 4 ); }
    Version maxVersion() const { return Version( 0, 9, 9999 ); }
    
    std::string pluginPath() const { return std::string( "components" ); }
    std::string displayName() const { return std::string( "foobar2000" ); }

    std::string id() const { return std::string( "foo3" ); }
    BootstrapType bootstrapType() const{ return NoBootstrap; }
    bool isPlatformSupported() const
    {
        #ifdef WIN32
            return true;
        #endif
        return false;
    }

    IPluginInfo* clone() const { return new FooBar09PluginInfo( *this ); }
};

#endif //FOOBAR09_PLUGIN_INFO_H_

