#include "UnicornSettings.h"
#include "UnicornApplication.h"
#include <lastfm/User.h>

QList<lastfm::User>
unicorn::Settings::userRoster() const
{
    const_cast<Settings*>(this)->beginGroup( "Users" );
    QList<User> ret;
    foreach( QString child, childGroups()) {
        if( child == "com" || !contains( child + "/SessionKey" )) continue;
        ret << User( child );
    }
    const_cast<Settings*>(this)->endGroup();
    return ret;
}

unicorn::AppSettings::AppSettings( QString appname )
    : QSettings( unicorn::organizationName(), appname.isEmpty() ? qApp->applicationName() : appname )
{}

unicorn::UserSettings::UserSettings( QString username )
{
    beginGroup( "Users" );
    beginGroup( username );
}
