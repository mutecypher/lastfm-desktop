
#include <QStringList>

#include "WmpPluginInfo.h"

unicorn::WmpPluginInfo::WmpPluginInfo( QObject* parent )
    :IPluginInfo( parent )
{}

unicorn::Version
unicorn::WmpPluginInfo::version() const
{
    return Version( 2, 1, 0, 8 );
}

QString
unicorn::WmpPluginInfo::name() const
{
    return "Windows Media Player";
}

QString
unicorn::WmpPluginInfo::displayName() const
{
    return QString( "MPlayer2" );
}

QString
unicorn::WmpPluginInfo::processName() const
{
    return QString( "wmplayer.exe" );
}

QString
unicorn::WmpPluginInfo::id() const
{
    return "wmp";
}

bool
unicorn::WmpPluginInfo::isAppInstalled() const
{
    return QSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\MediaPlayer\\Preferences", QSettings::NativeFormat).contains("FirstRun");
}

unicorn::IPluginInfo::BootstrapType
unicorn::WmpPluginInfo::bootstrapType() const
{
    return PluginBootstrap;
}

QString
unicorn::WmpPluginInfo::pluginInstaller() const
{
    return "WmpPluginSetup_2.1.0.8.exe";
}
