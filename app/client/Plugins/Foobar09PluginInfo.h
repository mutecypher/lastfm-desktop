#ifndef FOOBAR09_PLUGIN_INFO_H_
#define FOOBAR09_PLUGIN_INFO_H_

#include <QSettings>

#include "IPluginInfo.h"

class FooBar09PluginInfo : public IPluginInfo
{
    Q_OBJECT
public:
    FooBar09PluginInfo( QObject* parent = 0 ) : IPluginInfo( parent ) {}

    std::string name() const { return "Foobar 2000 0.9.4"; }
    Version minVersion() const { return Version( 0, 9, 4 ); }
    Version maxVersion() const { return Version( 0, 9, 9999 ); }
    
    std::string pluginPath() const { return std::string( "components" ); }
    std::string displayName() const { return std::string( "foobar2000" ); }
    std::string processName() const { return std::string( "foobar2000.exe" ); }

    std::string id() const { return std::string( "foo3" ); }
    BootstrapType bootstrapType() const{ return NoBootstrap; }
    bool isPlatformSupported() const
    {
        #ifdef WIN32
            return true;
        #endif
        return false;
    }

    std::tstring pluginInstallPath() const
    {
#ifdef Q_OS_WIN32
        // This isn't the correct key, I'm not sure there is one
        QSettings settings( "HKEY_CURRENT_USER\\foobar2000", QSettings::NativeFormat );
        QString foobarLocation = settings.value( ".", QString::fromStdWString( programFilesX86().append( L"\\foobar2000") ) ).toString().append( "\\components" );
        return foobarLocation.toStdWString();
#endif
        Q_ASSERT( !"There is no windows mediaplayer on non-windows platforms!" );
        return std::tstring();
    }
};

#endif //FOOBAR09_PLUGIN_INFO_H_

