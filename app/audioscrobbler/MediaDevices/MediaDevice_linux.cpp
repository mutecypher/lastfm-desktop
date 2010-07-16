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


