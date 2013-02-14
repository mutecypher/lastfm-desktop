
#include <QStringList>

#include "ITunesPluginInfo.h"

unicorn::ITunesPluginInfo::ITunesPluginInfo( QObject* parent )
    :IPluginInfo( parent )
{}

unicorn::Version
unicorn::ITunesPluginInfo::version() const
{
    return Version( 6, 0, 5, 4 );
}

QString
unicorn::ITunesPluginInfo::name() const
{
    return "iTunes";
}

QString
unicorn::ITunesPluginInfo::displayName() const
{
    return QString( "iTunes" );
}

QString
unicorn::ITunesPluginInfo::processName() const
{
    return QString( "iTunes.exe" );
}

QString
unicorn::ITunesPluginInfo::id() const
{
#ifdef Q_OS_WIN32
    return "itw";
#elif
    return "osx";
#endif
}

bool
unicorn::ITunesPluginInfo::isAppInstalled() const
{
#ifdef Q_OS_WIN32
    return QSettings("HKEY_CURRENT_USER\\Software\\Apple Computer, Inc.\\iTunes", QSettings::NativeFormat).contains("SM Shortcut Installed");
#elif
    return true;
#endif
}

unicorn::IPluginInfo::BootstrapType
unicorn::ITunesPluginInfo::bootstrapType() const
{
    return ClientBootstrap;
}

QString
unicorn::ITunesPluginInfo::pluginInstaller() const
{
    return "iTunesPluginWinSetup_6.0.5.4.exe";
}

