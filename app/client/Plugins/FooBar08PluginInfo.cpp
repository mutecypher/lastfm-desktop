
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
    return "Foobar 2000 0.9";
}

Version
FooBar08PluginInfo::minVersion() const
{
    return Version( 0, 9 );
}

Version
FooBar08PluginInfo::maxVersion() const
{
    return Version( 0, 9, 3, 9999 );
}

QString
FooBar08PluginInfo::pluginPath() const
{
    return QString( "components" );
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

IPluginInfo::BootstrapType
FooBar08PluginInfo::bootstrapType() const
{
    return NoBootstrap;
}

QString
FooBar08PluginInfo::pluginInstallPath() const
{
    // This isn't the correct key, I'm not sure there is one
    QSettings settings( "HKEY_CURRENT_USER\\foobar2000", QSettings::NativeFormat );
    QString foobarLocation = settings.value( ".", programFilesX86().append( "\\foobar2000") ).toString().append( "\\components" );
    return foobarLocation;
}

QString
FooBar08PluginInfo::pluginInstaller() const
{
    return "FooPlugin0.9Setup_2.1.exe";
}
