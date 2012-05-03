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


#include "lib/unicorn/QMessageBoxBuilder.h"
#include "lib/unicorn/UnicornSettings.h"
#include "lib/unicorn/UnicornSession.h"

#include <QApplication>
#include <QByteArray>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>

#include "IpodDevice.h"

IpodDevice::IpodDevice( const QString& deviceId, const QString& deviceName )
    : m_deviceId( deviceId ), m_deviceName( deviceName )
{}

QString
IpodDevice::deviceId() const
{
    return m_deviceId;
}

QString
IpodDevice::deviceName() const
{
    return m_deviceName;
}

void
IpodDevice::setScrobble( bool scrobble )
{
    setSetting( "scrobble", scrobble );
}

bool
IpodDevice::scrobble() const
{
    return const_cast<IpodDevice*>(this)->setting( "scrobble", false ).toBool();
}


void
IpodDevice::setAlwaysAsk( bool alwaysAsk )
{
    setSetting( "alwaysAsk", alwaysAsk );
}

bool
IpodDevice::alwaysAsk() const
{
    return const_cast<IpodDevice*>(this)->setting( "alwaysAsk", true ).toBool();
}


void
IpodDevice::setSetting( QString key, QVariant value )
{
    // Find the setting for this
    QList<lastfm::User> roster = unicorn::Settings().userRoster();

    bool found = false;

    foreach( lastfm::User user, roster )
    {
        unicorn::UserSettings us( user.name() );
        int count = us.beginReadArray( "associatedDevices" );

        for ( int i = 0; i < count; i++ )
        {
            us.setArrayIndex( i );

            if ( us.value( "deviceId" ).toString() == m_deviceId )
            {
                us.setValue( key, value );
                found = true;
                break;
            }
        }
        us.endArray();

        if ( found ) break;
    }
}

QVariant
IpodDevice::setting( QString key, QVariant defaultValue )
{
    QVariant value;

    // Find the setting for this
    QList<lastfm::User> roster = unicorn::Settings().userRoster();

    bool found = false;

    foreach( lastfm::User user, roster )
    {
        unicorn::UserSettings us( user.name() );
        int count = us.beginReadArray( "associatedDevices" );

        for ( int i = 0; i < count; i++ )
        {
            us.setArrayIndex( i );

            if ( us.value( "deviceId" ).toString() == m_deviceId )
            {
                value = us.value( key, defaultValue );
                found = true;
                break;
            }
        }
        us.endArray();

        if ( found ) break;
    }

    return value;
}
