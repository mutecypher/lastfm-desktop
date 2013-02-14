
#include <QSettings>
#include <QStringList>

#include "FooBar09PluginInfo.h"

unicorn::FooBar09PluginInfo::FooBar09PluginInfo( QObject* parent )
    :IPluginInfo( parent )
{}

unicorn::Version
unicorn::FooBar09PluginInfo::version() const
{
    return Version( 2, 3, 1, 3 );
}

QString
unicorn::FooBar09PluginInfo::name() const
{
    return "foobar2000";
}

QString
unicorn::FooBar09PluginInfo::displayName() const
{
    return QString( "foobar2000" );
}

QString
unicorn::FooBar09PluginInfo::processName() const
{
    return QString( "foobar2000.exe" );
}

QString
unicorn::FooBar09PluginInfo::id() const
{
    return QString( "foo3" );
}

bool
unicorn::FooBar09PluginInfo::isAppInstalled() const
{
    return QSettings("HKEY_CURRENT_USER\\Software\\foobar2000", QSettings::NativeFormat).contains("DefaultShellAction");
}

unicorn::IPluginInfo::BootstrapType
unicorn::FooBar09PluginInfo::bootstrapType() const
{
    return NoBootstrap;
}

QString
unicorn::FooBar09PluginInfo::pluginInstaller() const
{
    return "FooPlugin0.9.4Setup_2.3.1.3.exe";
}

