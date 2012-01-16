
#include <QStringList>

#include "ITunesPluginInfo.h"

ITunesPluginInfo::ITunesPluginInfo( QObject* parent )
    :IPluginInfo( parent )
{}

Version
ITunesPluginInfo::version() const
{
    return Version( 5, 0, 1, 0 );
}

QString
ITunesPluginInfo::name() const
{
    return "iTunes";
}

Version
ITunesPluginInfo::minVersion() const
{
    return Version();
}

Version
ITunesPluginInfo::maxVersion() const
{
    return Version();
}

QString
ITunesPluginInfo::pluginPath() const
{
    return QString( "Plug-Ins" );
}

QString
ITunesPluginInfo::displayName() const
{
    return QString( "iTunes" );
}

QString
ITunesPluginInfo::processName() const
{
    return QString( "iTunes.exe" );
}

QString
ITunesPluginInfo::id() const
{
#ifdef Q_OS_WIN32
    return "itw";
#elif
    return "osx";
#endif
}

IPluginInfo::BootstrapType
ITunesPluginInfo::bootstrapType() const
{
    return ClientBootstrap;
}

QString ITunesPluginInfo::pluginInstallPath() const
{
    return programFilesX86() + "\\iTunes\\Plug-Ins";
}

QString
ITunesPluginInfo::pluginInstaller() const
{
    return "iTunesPluginWinSetup_5.0.0.0.exe";
}

