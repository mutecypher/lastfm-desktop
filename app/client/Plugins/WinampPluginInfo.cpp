
#include <QStringList>

#include "WinampPluginInfo.h"

WinampPluginInfo::WinampPluginInfo( QObject* parent )
    :IPluginInfo( parent )
{}

Version
WinampPluginInfo::version() const
{
    return Version( 2, 1, 0, 9 );
}

QString
WinampPluginInfo::name() const
{
    return "Winamp";
}

Version
WinampPluginInfo::minVersion() const
{
    return Version();
}

Version
WinampPluginInfo::maxVersion() const
{
    return Version();
}

QString
WinampPluginInfo::pluginPath() const
{
    return QString( "plugins" );
}

QString
WinampPluginInfo::displayName() const
{
    return QString( "MPlayer2" );
}

QString
WinampPluginInfo::processName() const
{
    return QString( "winamp.exe" );
}

QString
WinampPluginInfo::id() const
{
    return "wa2";
}

IPluginInfo::BootstrapType
WinampPluginInfo::bootstrapType() const
{
    return PluginBootstrap;
}

QString
WinampPluginInfo::pluginInstallPath() const
{
    QSettings settings( "HKEY_CURRENT_USER\\Software\\Winamp", QSettings::NativeFormat );
    QString winampFolder = settings.value( ".", programFilesX86().append( "/Winamp" ) ).toString().append( "/plugins" );
    return winampFolder;
}

QString
WinampPluginInfo::pluginInstaller() const
{
    return "WinampPluginSetup_2.1.0.9.exe";
}
