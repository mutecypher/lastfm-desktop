#ifndef WINAMP_PLUGIN_INFO_H_
#define WINAMP_PLUGIN_INFO_H_

#include "../../lib/unicorn/Updater/IPluginInfo.h"
#include "../../lib/DllExportMacro.h"

class UNICORN_DLLEXPORT WinampPluginInfo : public IPluginInfo
{
public:
    std::string name() const { return "Winamp"; }
    Version minVersion() const { return Version(); }
    Version maxVersion() const { return Version(); }
    
    std::string pluginPath() const { return std::string( "plugins" ); }
    std::string displayName() const { return std::string( "MPlayer2" ); }
    std::string processName() const { return std::string( "winamp.exe" ); }

    std::string id() const { return "wa2"; }
    BootstrapType bootstrapType() const { return PluginBootstrap; }
    bool isPlatformSupported() const
    {
        #ifdef WIN32
            return true;
        #endif
        return false;
    }

    IPluginInfo* clone() const { return new WinampPluginInfo( *this ); }

#ifdef QT_VERSION
    QString pluginInstallPath() const
    {
    #ifdef Q_OS_WIN
        QSettings s( QString( "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows"
                              "\\CurrentVersion\\App Paths\\") + QString::fromStdString( processName() ),
                     QSettings::NativeFormat );
        return s.value( "Path" ).toString() + "\\Plugins";
    #endif 
        Q_ASSERT( !"There is no winamp on non-windows platforms!" );
        return "";
    }
#endif

};

#endif //WINAMP_PLUGIN_INFO_H_

