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

#include <QStringList>
#include <QRegExp>

#include "_version.h"
#include "Application.h"
#include "app/moose.h"

int main( int argc, char** argv )
{
    QCoreApplication::setApplicationName( "Last.fm Scrobbler" );
    QCoreApplication::setApplicationVersion( VERSION );

    try
    {
        audioscrobbler::Application app( argc, argv );

        if ( app.sendMessage( app.arguments() ) || app.arguments().contains("--exit") )
            return 0;

        // It's possible that we were unable to send the
        // message, but the app is actually running
        if ( app.isRunning() )
            return 0;

        app.init();
        return app.exec();
    }
    catch (unicorn::Application::StubbornUserException&)
    {
        // user wouldn't log in
        return 0;
    }
}
