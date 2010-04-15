#include "UnicornSettings.h"
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
