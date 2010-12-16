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

#ifdef QT_VERSION
    QString pluginInstallPath() const
    {
    #ifdef Q_OS_WIN
        QSettings s( QString( "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows"
                              "\\CurrentVersion\\App Paths\\") + QString::fromStdString( processName() ),
                     QSettings::NativeFormat );
        return s.value( "Path" ).toString() + "\\Plugins";
    #endif 
        return "~/Library/iTunes/iTunes Plug-ins";
    }
#endif



};

#endif //ITUNES_PLUGIN_INFO_H_

