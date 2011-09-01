/*
   Copyright 2005-2010 Last.fm Ltd.
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
#include "MediaDevice.h"
#include "lib/unicorn/UnicornSettings.h"

#include <core/misc.h>

#include <QSqlQuery>
#include <QSqlError>

MediaDevice::MediaDevice()
{
}

bool
MediaDevice::associateDevice( QString username )
{
    if ( deviceId().isEmpty() || deviceName().isEmpty() )
        return false;

#ifdef Q_WS_X11
    if ( mountPath().isEmpty() || !QFile::exists( mountPath() ) )
        return false;
#endif

    unicorn::UserSettings us( username );
    int count = us.beginReadArray( "associatedDevices" );
    us.endArray();

    us.beginWriteArray( "associatedDevices" );
    us.setArrayIndex( count );
    us.setValue( "deviceId", deviceId() );
    us.setValue( "deviceName", deviceName() );

#ifdef Q_WS_X11
    us.setValue( "mountPath", QDir::toNativeSeparators( mountPath() ) );
#endif

    us.endArray();
    return true;
}

bool
MediaDevice::isDeviceKnown() const
{
    unicorn::UserSettings us;
    int count = us.beginReadArray( "associatedDevices" );
    QString devId;
    bool isKnown = false;
    for ( int i = 0; i < count; i++ )
    {
        us.setArrayIndex( i );
        devId = us.value( "deviceId", "" ).toString();
        if ( devId == deviceId() )
        {
            isKnown = true;
        }
    }
    us.endArray();
    return isKnown;
}
