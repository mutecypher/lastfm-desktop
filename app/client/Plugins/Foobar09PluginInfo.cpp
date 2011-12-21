
#include <QSettings>
#include <QStringList>

#include "FooBar09PluginInfo.h"

FooBar09PluginInfo::FooBar09PluginInfo( QObject* parent )
    :IPluginInfo( parent )
{}

Version
FooBar09PluginInfo::version() const
{
    return Version( 2, 3, 1, 2 );
}

QString
FooBar09PluginInfo::name() const
{
    return "Foobar 2000 0.9.4";
}

Version
FooBar09PluginInfo::minVersion() const
{
    return Version( 0, 9, 4 );
}

Version
FooBar09PluginInfo::maxVersion() const
{
    return Version( 0, 9, 9999 );
}

QString
FooBar09PluginInfo::pluginPath() const
{
    return QString( "components" );
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

IPluginInfo::BootstrapType
FooBar09PluginInfo::bootstrapType() const
{
    return NoBootstrap;
}

QString FooBar09PluginInfo::pluginInstallPath() const
{
    // This isn't the correct key, I'm not sure there is one
    QSettings settings( "HKEY_CURRENT_USER\\foobar2000", QSettings::NativeFormat );
    QString foobarLocation = settings.value( ".", programFilesX86().append( "\\foobar2000" ) ).toString().append( "\\components" );
    return foobarLocation;
}

QString
FooBar09PluginInfo::pluginInstaller() const
{
    return "FooPlugin0.9.4Setup_2.3.1.2.exe";
}

