#include "UnicornSettings.h"
#include "UnicornApplication.h"
#include <lastfm/User>

QList<lastfm::User>
unicorn::Settings::userRoster() const
{
    QList<User> ret;
    foreach( QString child, childGroups()) {
        if( child == "com" || !contains( child + "/SessionKey" )) continue;
        ret << User( child );
    }
    return ret;
}

unicorn::AppSettings::AppSettings( QString appname )
    : QSettings( unicorn::organizationName(), appname.isEmpty() ? qApp->applicationName() : appname )
{}

unicorn::UserSettings::UserSettings( QString username )
{
    beginGroup( username );
}
