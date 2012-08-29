
#include <QSettings>
#include <QStringList>

#include "FooBar09PluginInfo.h"

FooBar09PluginInfo::FooBar09PluginInfo( QObject* parent )
    :IPluginInfo( parent )
{}

Version
FooBar09PluginInfo::version() const
{
    return Version( 2, 3, 1, 3 );
}

QString
FooBar09PluginInfo::name() const
{
    return "foobar2000";
}

QString
FooBar09PluginInfo::displayName() const
{
    return QString( "foobar2000" );
}

QString
FooBar09PluginInfo::processName() const
{
    return QString( "foobar2000.exe" );
}

QString
FooBar09PluginInfo::id() const
{
    return QString( "foo3" );
}

bool
FooBar09PluginInfo::isAppInstalled() const
{
    return QSettings("HKEY_CURRENT_USER\\Software\\foobar2000", QSettings::NativeFormat).contains("DefaultShellAction");
}

IPluginInfo::BootstrapType
FooBar09PluginInfo::bootstrapType() const
{
    return NoBootstrap;
}

QString
FooBar09PluginInfo::pluginInstaller() const
{
    return "FooPlugin0.9.4Setup_2.3.1.3.exe";
}

