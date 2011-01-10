#ifndef ITUNES_PLUGIN_INFO_H_
#define ITUNES_PLUGIN_INFO_H_

#include "../../lib/unicorn/Updater/IPluginInfo.h"
#include "../../lib/DllExportMacro.h"

class UNICORN_DLLEXPORT ITunesPluginInfo : public IPluginInfo
{
public:
    std::string name() const { return "iTunes"; }
    Version minVersion() const { return Version(); }
    Version maxVersion() const { return Version(); }
    
    std::string pluginPath() const { return std::string( "Plug-Ins" ); }
    std::string displayName() const { return std::string( "iTunes" ); }
    std::string processName() const { return std::string( "iTunes.exe" ); }

    std::string id() const 
    { 
        #ifdef WIN32    
            return "itw"; 
        #endif
        return "osx";
    }
    BootstrapType bootstrapType() const { return ClientBootstrap; }

    bool isPlatformSupported() const
    {
        #if defined (WIN32) || defined (__APPLE__)
            return true;
        #endif
        return false;
    }

    IPluginInfo* clone() const { return new ITunesPluginInfo( *this ); }

	std::tstring pluginInstallPath() const
    {
		return std::tstring();
    }

};

#endif //ITUNES_PLUGIN_INFO_H_

