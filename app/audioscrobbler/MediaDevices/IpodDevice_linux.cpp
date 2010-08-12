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
#include "IpodDevice_linux.h"
#include "lib/unicorn/QMessageBoxBuilder.h"
#include "lib/unicorn/UnicornSettings.h"
#include "lib/unicorn/UnicornSession.h"

#include <QApplication>
#include <QByteArray>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>

extern "C"
{
    #include <gpod/itdb.h>
    #include <glib/glist.h>
}

TracksFetcher::TracksFetcher( Itdb_iTunesDB *itdb, IpodDevice* parent )
{
    m_itdb = itdb;
    m_ipod = parent;
}

void
TracksFetcher::run()
{
    fetchTracks();
    exec();
}

void
TracksFetcher::fetchTracks()
{
    GList *cur;
    for ( cur = m_itdb->tracks; cur; cur = cur->next )
    {
        Itdb_Track *iTrack = ( Itdb_Track * )cur->data;
        if ( !iTrack )
            continue;

        int newPlayCount = iTrack->playcount - m_ipod->previousPlayCount( iTrack );
        QDateTime time;
        time.setTime_t( iTrack->time_played );

        if ( time.toTime_t() == 0 )
            continue;

        QDateTime prevPlayTime = m_ipod->previousPlayTime( iTrack );

        //this logic takes into account that sometimes the itdb track play count is not
        //updated correctly (or libgpod doesn't get it right),
        //so we rely on the track play time too, which seems to be right most of the time
        if ( ( iTrack->playcount > 0 && newPlayCount > 0 ) || time > prevPlayTime )
        {
            Track lstTrack;
            m_ipod->setTrackInfo( lstTrack, iTrack );

            if ( newPlayCount == 0 )
                newPlayCount++;

            //add the track to the list as many times as the updated playcount.
            for ( int i = 0; i < newPlayCount; i++ )
            {
                m_tracksToScrobble.append( lstTrack );
            }

            m_ipod->commit( iTrack );
        }
    }
    exit();
}


IpodDevice::IpodDevice()
    : m_itdb( 0 )
    , m_mpl( 0 )
{}


IpodDevice::~IpodDevice()
{
    if ( m_itdb )
    {
        itdb_free( m_itdb );
        itdb_playlist_free( m_mpl );
    }
}

bool
IpodDevice::deleteDeviceHistory( QString username, QString deviceId )
{
    QString const name = DB_NAME;
    QSqlDatabase db = QSqlDatabase::database( name );
    bool b = false;

    if ( !db.isValid() )
    {
        db = QSqlDatabase::addDatabase( "QSQLITE", name );
        db.setDatabaseName( lastfm::dir::runtimeData().filePath( name + ".db" ) );
    }

    db.open();

    QSqlQuery q( db );
    b = q.exec( "DROP TABLE " + username + "_" + deviceId );

    if ( !b )
        qWarning() << q.lastError().text();

    return b;
}

bool
IpodDevice::deleteDevicesHistory()
{
    QString const name = DB_NAME;
    QString filePath = lastfm::dir::runtimeData().filePath( name + ".db" );
    return QFile::remove( filePath );
}


QSqlDatabase
IpodDevice::database() const
{
    QString const name = DB_NAME;
    QSqlDatabase db = QSqlDatabase::database( name );

    if ( !db.isValid() )
    {
        db = QSqlDatabase::addDatabase( "QSQLITE", name );
        db.setDatabaseName( lastfm::dir::runtimeData().filePath( name + ".db" ) );

        db.open();


    }

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

    return db;
}

void
IpodDevice::open()
{
    QByteArray _mountpath = QFile::encodeName( mountPath() );
    const char* mountpath = _mountpath.data();

    qDebug() << "mount path: " << mountPath();

    m_itdb = itdb_new();
    itdb_set_mountpoint( m_itdb, mountpath );
    m_mpl = itdb_playlist_new( "iPod", false );
    itdb_playlist_set_mpl( m_mpl );
    GError* err = 0;
    m_itdb = itdb_parse( mountpath, &err );

    if ( err )
        throw tr( "The iPod database could not be opened." );

    if( m_deviceId.isEmpty() )
    {
        const Itdb_IpodInfo* iPodInfo = itdb_device_get_ipod_info( m_itdb->device );
        const gchar* ipodModel = itdb_info_get_ipod_model_name_string( iPodInfo->ipod_model );
        m_ipodModel = QString( ipodModel );
        m_deviceId = m_ipodModel.section( ' ', 0, 0 ) + "_" + QString::number( m_itdb->id );
    }

}

QList<Track>
IpodDevice::tracksToScrobble()
{
    return m_tracksToScrobble;
}

void
IpodDevice::onFinished()
{
    m_tracksToScrobble = m_tf->tracksToScrobble();
    emit scrobblingCompleted( m_tracksToScrobble.count() );
}

void
IpodDevice::fetchTracksToScrobble()
{
    try
    {
        open();
    }
    catch ( QString &error )
    {
        m_error = error;
        emit errorOccurred();
        return;
    }

    if ( !m_itdb )
    {
        m_error = "Itunes Database error";
        emit errorOccurred();
        return;
    }
    
    emit calculatingScrobbles( itdb_tracks_number( m_itdb ) );
    m_tf = new TracksFetcher( m_itdb, this );
    connect( m_tf, SIGNAL( finished() ), this, SLOT( onFinished() ) );
    m_tf->start();
   /* GList *cur;
    for ( cur = m_itdb->tracks; cur; cur = cur->next )
    {
        Itdb_Track *iTrack = ( Itdb_Track * )cur->data;
        if ( !iTrack )
            continue;

        int newPlayCount = iTrack->playcount - previousPlayCount( iTrack );
        QDateTime time;
        time.setTime_t( iTrack->time_played );

        if ( time.toTime_t() == 0 )
            continue;

        QDateTime prevPlayTime = previousPlayTime( iTrack );

        //this logic takes into account that sometimes the itdb track play count is not
        //updated correctly (or libgpod doesn't get it right),
        //so we rely on the track play time too, which seems to be right most of the time
        if ( ( iTrack->playcount > 0 && newPlayCount > 0 ) || time > prevPlayTime )
        {
            Track lstTrack;
            setTrackInfo( lstTrack, iTrack );

            if ( newPlayCount == 0 )
                newPlayCount++;

            //add the track to the list as many times as the updated playcount.
            for ( int i = 0; i < newPlayCount; i++ )
            {
                m_tracksToScrobble.append( lstTrack );
            }

            commit( iTrack );
        }
    }*/
    m_error = "";
//    emit scrobblingCompleted( m_tracksToScrobble.count() );
}


void
IpodDevice::setTrackInfo( Track& lstTrack, Itdb_Track* iTrack )
{
    MutableTrack( lstTrack ).setArtist( QString::fromUtf8( iTrack->artist ) );
    MutableTrack( lstTrack ).setAlbum( QString::fromUtf8( iTrack->album ) );
    MutableTrack( lstTrack ).setTitle( QString::fromUtf8( iTrack->title ) );
    MutableTrack( lstTrack ).setSource( Track::MediaDevice );

    QDateTime t;
    t.setTime_t( iTrack->time_played );
    MutableTrack( lstTrack ).setTimeStamp( t );
    MutableTrack( lstTrack ).setDuration( iTrack->tracklen / 1000 ); // set duration in seconds

    MutableTrack( lstTrack ).setExtra( "playerName", "iPod " + m_ipodModel );
}


uint
IpodDevice::previousPlayCount( Itdb_Track* track ) const
{
    QSqlDatabase db = database();
    QSqlQuery query( db );
    QString sql = "SELECT playcount FROM " + tableName() + " WHERE id=" + QString::number( track->id );

    query.exec( sql );

    if( query.next() )
        return query.value( 0 ).toUInt();
    return 0;
}

QDateTime
IpodDevice::previousPlayTime( Itdb_Track* track ) const
{
    QSqlDatabase db = database();
    QSqlQuery query( db );
    QString sql = "SELECT lastplaytime FROM " + tableName() + " WHERE id=" + QString::number( track->id );

    query.exec( sql );

    if( query.next() )
        return QDateTime::fromTime_t( query.value( 0 ).toUInt() );
    return QDateTime::fromTime_t( 0 );
}

void
IpodDevice::commit( Itdb_Track* iTrack )
{
    QSqlDatabase db = database();
    QSqlQuery query( db );
    QString sql = "REPLACE INTO " + tableName() + " ( playcount, lastplaytime, id ) VALUES( %1, %2, %3 )";

    query.exec( sql.arg( iTrack->playcount ).arg( iTrack->time_played ).arg( iTrack->id  ) );
    if( query.lastError().type() != QSqlError::NoError )
        qWarning() << query.lastError().text();
}


QString
IpodDevice::deviceName() const
{
    QStringList devPath = mountPath().split( "/", QString::SkipEmptyParts );
    if ( !devPath.isEmpty() )
        return devPath.last();
    return QString();
}

QString
IpodDevice::tableName() const
{
    return unicorn::Session().username() + "_" + m_deviceId;
}

bool
IpodDevice::autoDetectMountPath()
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
