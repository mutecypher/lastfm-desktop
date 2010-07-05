#include "MediaDevice.h"
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


