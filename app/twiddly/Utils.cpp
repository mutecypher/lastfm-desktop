/*
   Copyright 2005-2012 Last.fm Ltd.
      - Primarily authored by Michael Coffey

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



#include <QProcess>
#include <QSettings>

#include "lib/unicorn/UnicornSettings.h"
#include <lastfm/misc.h>

#include "Utils.h"

#ifndef Q_OS_MAC
void
Utils::startAudioscrobbler( QStringList& vargs )
{
    QString path = unicorn::Settings().value( "Path" ).toString();
    if ( path.size() == 0 )
    {
        path = QSettings("HKEY_LOCAL_MACHINE\\Software\\Last.fm\\Client", QSettings::NativeFormat ).value( "Path" ).toString();

        if ( path.size() == 0 )
        {
            path = lastfm::dir::programFiles().filePath( "Last.fm/Last.fm.exe" );
        }
    }

    QProcess::startDetached( path, vargs );
}

#endif // Q_OS_MAC
