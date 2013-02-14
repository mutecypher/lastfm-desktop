
#include <QStringList>

#include "FooBar08PluginInfo.h"

unicorn::FooBar08PluginInfo::FooBar08PluginInfo( QObject* parent )
    :IPluginInfo( parent )
{}

unicorn::Version
unicorn::FooBar08PluginInfo::version() const
{
    return Version( 2, 1, 0 );
}

QString
unicorn::FooBar08PluginInfo::name() const
{
    return "foobar2000";
}

QString
unicorn::FooBar08PluginInfo::displayName() const
{
    return QString( "foobar2000" );
}

QString
unicorn::FooBar08PluginInfo::processName() const
{
    return QString( "foobar2000.exe" );
}

QString
unicorn::FooBar08PluginInfo::id() const
{
    return QString( "foo2" );
}

bool
unicorn::FooBar08PluginInfo::isAppInstalled() const
{
    return QSettings("HKEY_CURRENT_USER\\Software\\foobar2000", QSettings::NativeFormat).contains("DefaultShellAction");
}

unicorn::IPluginInfo::BootstrapType
unicorn::FooBar08PluginInfo::bootstrapType() const
{
    return NoBootstrap;
}

QString
unicorn::FooBar08PluginInfo::pluginInstaller() const
{
    return "FooPlugin0.9Setup_2.1.exe";
}
