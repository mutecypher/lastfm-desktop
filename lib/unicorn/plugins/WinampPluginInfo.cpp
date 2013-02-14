
#include <QStringList>

#include "WinampPluginInfo.h"

unicorn::WinampPluginInfo::WinampPluginInfo( QObject* parent )
    :IPluginInfo( parent )
{}

unicorn::Version
unicorn::WinampPluginInfo::version() const
{
    return Version( 2, 1, 0, 11 );
}

QString
unicorn::WinampPluginInfo::name() const
{
    return "Winamp";
}

QString
unicorn::WinampPluginInfo::displayName() const
{
    return QString( "MPlayer2" );
}

QString
unicorn::WinampPluginInfo::processName() const
{
    return QString( "winamp.exe" );
}

QString
unicorn::WinampPluginInfo::id() const
{
    return "wa2";
}

bool
unicorn::WinampPluginInfo::isAppInstalled() const
{
    return QSettings("HKEY_CURRENT_USER\\Software\\Winamp", QSettings::NativeFormat).contains(".");
}

unicorn::IPluginInfo::BootstrapType
unicorn::WinampPluginInfo::bootstrapType() const
{
    return PluginBootstrap;
}

QString
unicorn::WinampPluginInfo::pluginInstaller() const
{
    return "WinampPluginSetup_2.1.0.11.exe";
}
