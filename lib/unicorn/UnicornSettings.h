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
#ifndef UNICORN_SETTINGS_H
#define UNICORN_SETTINGS_H

#include <QSettings>
#include <QString>
#include "UnicornApplication.h"

namespace lastfm{ class User; }
namespace unicorn
{
    inline const char* organizationName() { return "Last.fm"; }
    inline const char* organizationDomain() { return "last.fm"; }

    class Settings : public QSettings
    {
    public:
        Settings() : QSettings( unicorn::organizationName() )
        {}
        
        QList<lastfm::User> userRoster();

    };

    class AppSettings : public QSettings
    {
    public:
        AppSettings( QString appname = qApp->applicationName() ) : QSettings( unicorn::organizationName(), appname )
        {}
    };


    /** Clearly no use until a username() has been assigned. But this is
      * automatic if you use unicorn::Application anyway. */
    class UserSettings : public QSettings
    {
    public:
        UserSettings()
        {
            QString const username = Application::instance()->currentSession().username();
            beginGroup( username );
            // it shouldn't be possible, since unicorn::Application enforces
            // assignment of the username parameter before anything else
            Q_ASSERT( !username.isEmpty() );
        }

        static const char* subscriptionKey() { return "subscription"; }
    };
}


#endif
