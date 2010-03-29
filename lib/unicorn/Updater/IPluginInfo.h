#ifndef PLUGIN_INFO_H_
#define PLUGIN_INFO_H_

#include "lib/DllExportMacro.h"

class Version
{
public:
    Version(): m_valid( false ){}

    Version( unsigned int major, unsigned int minor = 0, unsigned int build = 0, unsigned int revision = 0 )
    :m_major(major), m_minor(minor), m_build(build), m_revision(revision), m_valid( true ){}

    bool isValid() const { return m_valid; }

    bool operator !=( const Version& that ) const{ return !(*this == that); }
    bool operator ==( const Version& that ) const
    {
        if( !m_valid || !that.m_valid ) return true;

        if( m_major != that.m_major ) return false;
        if( m_minor != that.m_minor ) return false;
        if( m_build != that.m_build ) return false;
        if( m_revision != that.m_revision ) return false;
        
        return true;
    }

    bool operator <( const Version& that ) const { return !(*this == that || *this > that ); }
    bool operator >( const Version& that ) const
    {
        if( !m_valid || !that.m_valid ) return true;

        if( m_major > that.m_major ) return true;
        if( m_minor > that.m_minor ) return true;
        if( m_build > that.m_build ) return true;
        if( m_revision > that.m_revision ) return true;
        
        return false;
    }

private:
    unsigned int m_major, m_minor, m_build, m_revision;
    bool m_valid;
};


class UNICORN_DLLEXPORT IPluginInfo
{
public:
    IPluginInfo();
    virtual ~IPluginInfo();

    enum BootstrapType{ NoBootstrap = 0, ClientBootstrap, PluginBootstrap };

    virtual std::string name() const = 0;

    virtual Version minVersion() const = 0;
    virtual Version maxVersion() const = 0;

    virtual std::string id() const = 0;
    virtual BootstrapType bootstrapType() const = 0;


    // Plugin install path relative to media player's base install path
    virtual std::string pluginPath() const = 0;

    // DisplayName string value as found in the HKEY_LM/Software/Microsoft/CurrentVersion/Uninstall/{GUID}/
    virtual std::string displayName() const = 0;

    // Return true if this plugin is supported on the current platform
    virtual bool isPlatformSupported() const = 0;

#ifdef QT_VERSION
    virtual bool isAppInstalled() const;
    virtual bool isInstalled() const;
    bool canBootstrap() const;
#endif //QT_VERSION
};

#endif //PLUGIN_INFO_H_
