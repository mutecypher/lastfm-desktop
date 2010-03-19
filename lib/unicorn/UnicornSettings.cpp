#include "UnicornSettings.h"
#include <lastfm/User>

QList<lastfm::User>
unicorn::Settings::userRoster()
{
    QList<User> ret;
    foreach( QString child, childGroups()) {
        if( child == "com" ) continue;
        ret << User( child );
    }
    return ret;
}
