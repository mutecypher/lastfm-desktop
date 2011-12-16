#ifndef WINAMP_PLUGIN_INFO_H_
#define WINAMP_PLUGIN_INFO_H_

#include <QSettings>
#include <QString>

#include "../Plugins/IPluginInfo.h"

class WinampPluginInfo : public IPluginInfo
{
    Q_OBJECT
public:
    WinampPluginInfo( QObject* parent = 0 ) : IPluginInfo( parent ) {}

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

    std::tstring pluginInstallPath() const
    {
#ifdef Q_OS_WIN32
        QSettings settings( "HKEY_CURRENT_USER\\Software\\Winamp", QSettings::NativeFormat );
        QString winampFolder = settings.value( ".", QString::fromStdWString( programFilesX86().append( L"\\Winamp" ) ) ).toString().append( "\\plugins" );
        return winampFolder.toStdWString();
#endif
        Q_ASSERT( !"There is no winamp on non-windows platforms!" );
        return std::tstring();
    }

};

#endif //WINAMP_PLUGIN_INFO_H_

