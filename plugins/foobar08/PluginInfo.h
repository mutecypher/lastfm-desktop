#ifndef FOOBAR08_PLUGIN_INFO_H_
#define FOOBAR08_PLUGIN_INFO_H_

#include "../../lib/unicorn/Updater/IPluginInfo.h"
#include "../../lib/DllExportMacro.h"

class UNICORN_DLLEXPORT FooBar08PluginInfo : public IPluginInfo
{
public:
    std::string name() const { return "Foobar 2000 0.9"; }
    Version minVersion() const { return Version( 0, 9 ); }
    Version maxVersion() const { return Version( 0, 9, 3, 9999 ); }
    
    std::string pluginPath() const { return std::string( "components" ); }
    std::string displayName() const { return std::string( "foobar2000" ); }
    std::string processName() const { return std::string( "foobar2000.exe" ); }

    std::string id() const { return std::string( "foo2" ); }
    BootstrapType bootstrapType() const { return NoBootstrap; }

    bool isPlatformSupported() const
    {
        #ifdef WIN32
            return true;
        #endif
        return false;
    }

    IPluginInfo* clone() const { return new FooBar08PluginInfo( *this ); }

#ifdef QT_VERSION
    QString pluginInstallPath() const
    {
    #ifdef Q_OS_WIN
        QSettings s( QString( "HKEY_LOCAL_MACHINE\\Software\\"
                              "\\foobar2000"), 
                     QSettings::NativeFormat );
        return s.value( "Install Path" ) + "\\components";
    #endif 
        Q_ASSERT( !"There is no windows mediaplayer on non-windows platforms!" );
    }
#endif
};

#endif //FOOBAR08_PLUGIN_INFO_H_

