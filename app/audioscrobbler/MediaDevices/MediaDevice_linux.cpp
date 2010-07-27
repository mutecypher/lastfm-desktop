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
#include "MediaDevice_linux.h"
#include "lib/unicorn/UnicornSettings.h"

#include <lastfm/misc.h>

#include <QSqlQuery>
#include <QSqlError>

MediaDevice::MediaDevice()
{
}

MediaDevice::~MediaDevice()
{}


QSqlDatabase
MediaDevice::database() const
{
    QString const name = "iPodTracks";
    QSqlDatabase db = QSqlDatabase::database( name );

    if ( !db.isValid() )
    {
        db = QSqlDatabase::addDatabase( "QSQLITE", name );
        db.setDatabaseName( lastfm::dir::runtimeData().filePath( name + ".db" ) );

        db.open();

        if( !db.tables().contains( tableName() ) )
        {
            QSqlQuery q( db );
            qDebug() << "table name: " << tableName();
            bool b = q.exec( "CREATE TABLE " + tableName() + " ( "
                             "id           INTEGER PRIMARY KEY, "
                             "playcount    INTEGER, "
                             "lastplaytime INTEGER )" );
            if ( !b )
                qWarning() << q.lastError().text();
        }

    }
    return db;
}

bool
MediaDevice::autoDetectMountPath()
{
    unicorn::UserSettings us;
    int count = us.beginReadArray( "associatedDevices" );

    if ( !count )
        return false;

    m_detectedDevices.clear();

    DeviceInfo deviceInfo;
    QString deviceId;
    for ( int i = 0; i < count; i++ )
    {
        us.setArrayIndex( i );
        deviceId = us.value( "deviceId" ).toString();
        deviceInfo.prettyName = us.value( "deviceName" ).toString();
        deviceInfo.mountPath = us.value( "mountPath" ).toString();
        if ( QFile::exists( deviceInfo.mountPath ) )
        {
            m_detectedDevices[ deviceId ] = deviceInfo;
        }
    }
    us.endArray();

    //No device detected or many, so user has to choose.
    if ( m_detectedDevices.count() == 0 || m_detectedDevices.count() > 1 )
    {
        return false;
    }

    setMountPath( m_detectedDevices.values()[ 0 ].mountPath );
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

bool
MediaDevice::associateDevice()
{
    if ( deviceId().isEmpty() || deviceName().isEmpty() || mountPath().isEmpty() || !QFile::exists( m_mountPath ) )
        return false;

    unicorn::UserSettings us;
    int count = us.beginReadArray( "associatedDevices" );
    us.endArray();

    us.beginWriteArray( "associatedDevices" );
    us.setArrayIndex( count );
    us.setValue( "deviceId", deviceId() );
    us.setValue( "deviceName", deviceName() );
    us.setValue( "mountPath", QDir::toNativeSeparators( mountPath() ) );
    us.endArray();
    return true;
}
