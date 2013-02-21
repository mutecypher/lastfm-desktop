/*
   Copyright 2005-2009 Last.fm Ltd.
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/

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
