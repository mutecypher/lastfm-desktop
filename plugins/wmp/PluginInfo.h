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
    std::string processName() const { return std::string( "wmplayer.exe" ); }

    std::string id() const { return "wmp"; }
    BootstrapType bootstrapType() const { return PluginBootstrap; }
    bool isPlatformSupported() const
    {
        #ifdef WIN32
            return true;
        #endif
        return false;
    }

    IPluginInfo* clone() const { return new WmpPluginInfo( *this ); }

#ifdef QT_VERSION
    QString pluginInstallPath() const
    {
    #ifdef Q_OS_WIN
        QSettings s( QString( "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows"
                              "\\CurrentVersion\\App Paths\\") + processName(), 
                     QSettings::NativeFormat );
        return s.value( "Path" ) + "\\Plugins";
    #endif 
        Q_ASSERT( !"There is no windows mediaplayer on non-windows platforms!" );
    }
#endif
};

#endif //WMP_PLUGIN_INFO_H_

