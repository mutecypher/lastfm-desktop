#pragma once

#include <lib/DllExportMacro.h>

#include <QString>

namespace unicorn
{

class UNICORN_DLLEXPORT Version
{
public:
    Version();

    Version( unsigned int major, unsigned int minor = 0, unsigned int build = 0, unsigned int revision = 0 );

    QString toString() const;
    static Version fromString( const QString& string );

    bool operator !=( const Version& that ) const;
    bool operator ==( const Version& that ) const;
    bool operator <( const Version& that ) const;
    bool operator >( const Version& that ) const;

private:
    unsigned int m_major;
    unsigned int m_minor;
    unsigned int m_build;
    unsigned int m_revision;
};

}
