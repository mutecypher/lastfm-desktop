#include "IpodDevice_linux.h"

#include <QApplication>
#include <QByteArray>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>

extern "C"
{
    #include <gpod/itdb.h>
    #include <glib/glist.h>
}

IpodDevice::IpodDevice()
    : m_itdb( 0 )
{}


IpodDevice::~IpodDevice()
{
    if ( m_itdb )
    {
        itdb_free( m_itdb );
    }
}


void
IpodDevice::open()
{
    QByteArray _mountpath = QFile::encodeName( m_mountPath );
    const char* mountpath = _mountpath.data();

    m_itdb = itdb_new();
    itdb_set_mountpoint( m_itdb, mountpath );
    GError* err = 0;
    m_itdb = itdb_parse( mountpath, &err );

    if ( err )
        throw tr( "The iPod database could not be opened." );

    if( m_tableName.isEmpty() )
    {
        const Itdb_IpodInfo* iPodInfo = itdb_device_get_ipod_info( m_itdb->device );
        const gchar* ipodModel = itdb_info_get_ipod_model_name_string( iPodInfo->ipod_model );
        m_ipodModel = QString( ipodModel );
        m_tableName = m_ipodModel.section( ' ', 0, 0 ) + "_" + QString::number( m_itdb->id );
    }
}


QList<Track>
IpodDevice::tracksToScrobble()
{
    QList<Track> tracks;
    try
    {
        open();
    }
    catch ( QString &error )
    {
        m_error = error;
        return tracks;
    }

    if ( !m_itdb )
        return tracks;


    GList *cur;
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
                tracks += lstTrack;
            }

            commit( iTrack );
        }
    }
    return tracks;
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
    qDebug() << "date time: " << t;
    qDebug() << "lstTrack timestamp: " << lstTrack.timestamp().toString();
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





