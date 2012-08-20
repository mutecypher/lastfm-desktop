
#include <QStringList>

#include "WmpPluginInfo.h"

WmpPluginInfo::WmpPluginInfo( QObject* parent )
    :IPluginInfo( parent )
{}

Version
WmpPluginInfo::version() const
{
    return Version( 2, 1, 0, 7 );
}

QString
WmpPluginInfo::name() const
{
    return "Windows Media Player";
}

Version
WmpPluginInfo::minVersion() const
{
    return Version( 9 );
}

Version
WmpPluginInfo::maxVersion() const
{
    return Version();
}

QString
WmpPluginInfo::pluginPath() const
{
    return QString( "Plugins" );
}

QString
WmpPluginInfo::displayName() const
{
    return QString( "MPlayer2" );
}

QString
WmpPluginInfo::processName() const
{
    return QString( "wmplayer.exe" );
}

QString
WmpPluginInfo::id() const
{
    return "wmp";
}

IPluginInfo::BootstrapType
WmpPluginInfo::bootstrapType() const
{
    return PluginBootstrap;
}

QString
WmpPluginInfo::pluginInstallPath() const
{
    return programFilesX86() + "\\Windows Media Player";
}

QString
WmpPluginInfo::pluginInstaller() const
{
    return "WmpPluginSetup_2.1.0.7.exe";
}
