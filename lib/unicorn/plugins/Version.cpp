#include "Version.h"

#include <QStringList>

unicorn::Version::Version()
    :m_major( 0 ),
      m_minor( 0 ),
      m_build( 0 ),
      m_revision( 0 )
{

}

unicorn::Version::Version( unsigned int major, unsigned int minor, unsigned int build, unsigned int revision )
    :m_major( major ),
      m_minor( minor ),
      m_build( build ),
      m_revision( revision )
{
}

bool
unicorn::Version::operator !=( const Version& that ) const
{
    return !(*this == that);
}

bool
unicorn::Version::operator ==( const Version& that ) const
{
    if( m_major != that.m_major ) return false;
    if( m_minor != that.m_minor ) return false;
    if( m_build != that.m_build ) return false;
    if( m_revision != that.m_revision ) return false;

    return true;
}

bool
unicorn::Version::operator <( const Version& that ) const
{
    return !(*this == that || *this > that );
}

bool
unicorn::Version::operator >( const Version& that ) const
{
    if( m_major > that.m_major ) return true;
    if( m_minor > that.m_minor ) return true;
    if( m_build > that.m_build ) return true;
    if( m_revision > that.m_revision ) return true;

    return false;
}

QString
unicorn::Version::toString() const
{
    return QString( "%1.%2.%3.%4" ).arg( QString::number( m_major ),
                                         QString::number( m_minor ),
                                         QString::number( m_build ),
                                         QString::number( m_revision ) );
}

unicorn::Version
unicorn::Version::fromString( const QString& string )
{
    QStringList versionList = string.split( "." );

    int major = versionList.count() > 0 ? versionList.at( 0 ).toUInt() : 0;
    int minor = versionList.count() > 1 ? versionList.at( 1 ).toUInt() : 0;
    int build = versionList.count() > 2 ? versionList.at( 2 ).toUInt() : 0;
    int revision = versionList.count() > 3 ? versionList.at( 3 ).toUInt() : 0;

    return Version( major, minor, build, revision );
}

