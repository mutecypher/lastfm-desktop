
#include <QStringList>

#include "FooBar08PluginInfo.h"

FooBar08PluginInfo::FooBar08PluginInfo( QObject* parent )
    :IPluginInfo( parent )
{}

Version
FooBar08PluginInfo::version() const
{
    return Version( 2, 1, 0 );
}

QString
FooBar08PluginInfo::name() const
{
    return "foobar2000";
}

QString
FooBar08PluginInfo::displayName() const
{
    return QString( "foobar2000" );
}

QString
FooBar08PluginInfo::processName() const
{
    return QString( "foobar2000.exe" );
}

QString
FooBar08PluginInfo::id() const
{
    return QString( "foo2" );
}

bool
FooBar08PluginInfo::isAppInstalled() const
{
    return QSettings("HKEY_CURRENT_USER\\Software\\foobar2000", QSettings::NativeFormat).contains("DefaultShellAction");
}

IPluginInfo::BootstrapType
FooBar08PluginInfo::bootstrapType() const
{
    return NoBootstrap;
}

QString
FooBar08PluginInfo::pluginInstaller() const
{
    return "FooPlugin0.9Setup_2.1.exe";
}
