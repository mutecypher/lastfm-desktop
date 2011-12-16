#ifndef PLUGIN_INFO_H_
#define PLUGIN_INFO_H_

#include <QString>
#include <QSettings>

#ifdef WIN32
	#include <windows.h>
	#include <tchar.h>
	#include <iostream>
	#include <Shlobj.h>
#endif

#if defined(WIN32) && defined(UNICODE)
	#define tstring wstring
#else
	#define tstring string
#endif

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


class IPluginInfo : public QObject
{
    Q_OBJECT
public:
    IPluginInfo( QObject* parent = 0);

    enum BootstrapType{ NoBootstrap = 0, ClientBootstrap, PluginBootstrap };

    virtual std::string name() const = 0;

    virtual Version minVersion() const = 0;
    virtual Version maxVersion() const = 0;

    virtual std::string id() const = 0;
    virtual BootstrapType bootstrapType() const = 0;

    virtual std::string processName() const = 0;

    // Plugin install path relative to media player's base install path
    virtual std::string pluginPath() const = 0;

    // DisplayName string value as found in the HKEY_LM/Software/Microsoft/CurrentVersion/Uninstall/{GUID}/
    virtual std::string displayName() const = 0;

    // Return true if this plugin is supported on the current platform
    virtual bool isPlatformSupported() const = 0;

    virtual std::tstring pluginInstallPath() const = 0;

#ifdef WIN32
    static BOOL isWow64()
    {
        BOOL bIsWow64 = FALSE;

        //IsWow64Process is not available on all supported versions of Windows.
        //Use GetModuleHandle to get a handle to the DLL that contains the function
        //and GetProcAddress to get a pointer to the function if available.

        void* fnIsWow64Process = GetProcAddress(
        GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

        if(NULL != fnIsWow64Process)
        {
            if (!IsWow64Process(GetCurrentProcess(),&bIsWow64))
            {
                //handle error
            }
        }
        return bIsWow64;
    }
#endif

#ifdef QT_VERSION
    virtual void restartProcess() const;
    virtual bool isAppInstalled() const;
    virtual bool isInstalled() const;
    bool canBootstrap() const;
#endif //QT_VERSION

protected:
#ifdef WIN32
    std::tstring programFilesX86() const
    {
        TCHAR path[MAX_PATH];
        SHGetSpecialFolderPath( NULL, path, CSIDL_PROGRAM_FILESX86, false );
        return path;
    }

    std::tstring programFiles64() const
    {
        TCHAR path[MAX_PATH];
        GetEnvironmentVariable( L"programw6432", path, 255);
        return path;
    }
#endif
};

#endif //PLUGIN_INFO_H_
