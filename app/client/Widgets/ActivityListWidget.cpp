#include <QApplication>
#include <QHeaderView>
#include <QScrollBar>

#include <lastfm/Track.h>

#include "lib/unicorn/dialogs/ShareDialog.h"
#include "lib/unicorn/dialogs/TagDialog.h"
#include "lib/unicorn/DesktopServices.h"

#include "../Services/ScrobbleService.h"
#include "../Application.h"

#include "TrackWidget.h"
#include "ActivityListWidget.h"

#define kScrobbleLimit 30

class ActivityListWidgetItem : public QListWidgetItem
{
public:
    ActivityListWidgetItem( QListWidget* parent );
    bool operator<( const QListWidgetItem& that ) const;

    bool isNowPlaying() const;
    void setNowPlaying( bool nowPlaying );

    void setTrack( lastfm::Track& track );

private:
    bool m_nowPlaying;
};

ActivityListWidgetItem::ActivityListWidgetItem( QListWidget* parent )
    :QListWidgetItem( parent ), m_nowPlaying( false )
{
}

bool
ActivityListWidgetItem::isNowPlaying() const
{
    return m_nowPlaying;
}

void
ActivityListWidgetItem::setNowPlaying( bool nowPlaying )
{
    m_nowPlaying = nowPlaying;
    static_cast<TrackWidget*>( listWidget()->itemWidget( this ) )->setNowPlaying( true );
}

void
ActivityListWidgetItem::setTrack( lastfm::Track& track )
{
    static_cast<TrackWidget*>( listWidget()->itemWidget( this ) )->setTrack( track );
}

bool
ActivityListWidgetItem::operator<( const QListWidgetItem& that ) const
{
    if ( m_nowPlaying )
        return true;
    else if ( static_cast<const ActivityListWidgetItem*>(&that)->m_nowPlaying )
        return false;

    return static_cast<TrackWidget*>( listWidget()->itemWidget( const_cast<ActivityListWidgetItem*>( this ) ) )->track().timestamp().toTime_t()
            > static_cast<TrackWidget*>( listWidget()->itemWidget( const_cast<QListWidgetItem*>( &that ) ) )->track().timestamp().toTime_t();
}

ActivityListWidget::ActivityListWidget( QWidget* parent )
    :QListWidget( parent ), m_trackItem( 0 )
{
    setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );

    connect( this, SIGNAL( clicked(QModelIndex) ), SLOT(onItemClicked(QModelIndex)));

#ifdef Q_OS_MAC
    connect( verticalScrollBar(), SIGNAL(valueChanged(int)), SLOT(scroll()) );
#endif

    setAttribute( Qt::WA_MacNoClickThrough );
    setAttribute( Qt::WA_MacShowFocusRect, false );

    setUniformItemSizes( false );
    setSortingEnabled( false );
    setSelectionMode( QAbstractItemView::NoSelection );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    connect( qApp, SIGNAL( sessionChanged( unicorn::Session* )), SLOT(onSessionChanged( unicorn::Session* )));

    connect( &ScrobbleService::instance(), SIGNAL(scrobblesCached(QList<lastfm::Track>)), SLOT(onScrobblesSubmitted(QList<lastfm::Track>) ) );
    connect( &ScrobbleService::instance(), SIGNAL(scrobblesSubmitted(QList<lastfm::Track>)), SLOT(onScrobblesSubmitted(QList<lastfm::Track>) ) );

    connect( &ScrobbleService::instance(), SIGNAL(trackStarted(Track,Track)), SLOT(onTrackStarted(Track,Track)));
    connect( &ScrobbleService::instance(), SIGNAL(paused()), SLOT(onPaused()));
    connect( &ScrobbleService::instance(), SIGNAL(resumed()), SLOT(onResumed()));
    connect( &ScrobbleService::instance(), SIGNAL(stopped()), SLOT(onStopped()));

    onSessionChanged( lastfm::ws::Username );
}

#ifdef Q_OS_MAC
void
ActivityListWidget::scroll()
{
    // KLUDGE: The friend list widgets don't move unless we do this
    sortItems( Qt::AscendingOrder );
}
#endif

void 
ActivityListWidget::onItemClicked( const QModelIndex& index ) 
{
    emit trackClicked( static_cast<TrackWidget*>( itemWidget( item( index.row() ) ) )->track() );
}

void
ActivityListWidget::onSessionChanged( unicorn::Session* session )
{
    onSessionChanged( session->userInfo().name() );
}

void
ActivityListWidget::onSessionChanged( const QString& username )
{
    if ( !username.isEmpty() )
    {
        QString path = lastfm::dir::runtimeData().filePath( username + "_recent_tracks.xml" );

        if ( m_path != path )
        {
            m_path = path;
            read();
        }

        refresh();
    }
}


void
ActivityListWidget::read()
{
    qDebug() << m_path;

    clear();

    // always have a now playing item in the list
    m_trackItem = new ActivityListWidgetItem( this );
    TrackWidget* trackWidget = new TrackWidget( m_track, this );
    trackWidget->setObjectName( "nowPlaying" );
    setItemWidget( m_trackItem, trackWidget );
    m_trackItem->setSizeHint( trackWidget->sizeHint() );
    m_trackItem->setHidden( true );
    m_trackItem->setNowPlaying( true );

    QFile file( m_path );
    file.open( QFile::Text | QFile::ReadOnly );
    QTextStream stream( &file );
    stream.setCodec( "UTF-8" );

    QDomDocument xml;
    xml.setContent( stream.readAll() );

    QList<Track> tracks;

    for (QDomNode n = xml.documentElement().lastChild(); !n.isNull(); n = n.previousSibling())
        tracks << Track( n.toElement() );

    addTracks( tracks );

    // Make sure we fetch info for any tracks with unknown loved status
    foreach ( const lastfm::Track& track, tracks )
        if ( track.loveStatus() == lastfm::Unknown )
            track.getInfo( this, "write", User().name() );

    limit( kScrobbleLimit );
}

void
ActivityListWidget::write()
{
    if ( !m_writeTimer )
    {
        m_writeTimer = new QTimer( this );
        connect( m_writeTimer, SIGNAL(timeout()), this, SLOT(doWrite()) );
        m_writeTimer->setSingleShot( true );
    }

    m_writeTimer->start( 500 );
}

void
ActivityListWidget::doWrite()
{
    qDebug() << "Writing recent_tracks";

    if ( count() == 0 )
        QFile::remove( m_path );
    else
    {
        QDomDocument xml;
        QDomElement e = xml.createElement( "recent_tracks" );
        e.setAttribute( "product", QCoreApplication::applicationName() );
        e.setAttribute( "version", "2" );

        for ( int i = 0 ; i < count() ; ++i )
        {
            if ( !static_cast<ActivityListWidgetItem*>( item( i ) )->isNowPlaying() )
                e.appendChild( static_cast<TrackWidget*>( itemWidget( item( i ) ) )->track().toDomElement( xml ) );
        }

        xml.appendChild( e );

        QFile file( m_path );
        file.open( QIODevice::WriteOnly | QIODevice::Text );

        QTextStream stream( &file );
        stream.setCodec( "UTF-8" );
        stream << "<?xml version='1.0' encoding='utf-8'?>\n";
        stream << xml.toString( 2 );
    }
}

void
ActivityListWidget::onTrackStarted( const Track& track, const Track& )
{
    // Don't display Spotify here as we don't know if the current user is the one scrobbling
    // If it is the current user it will be fetch by user.getRecentTracks
    if ( track.extra( "playerId" ) != "spt" )
    {
        m_track = track;
        m_trackItem->setTrack( m_track );
        m_trackItem->setHidden( false );

        connect( m_track.signalProxy(), SIGNAL(loveToggled(bool)), SLOT(write()));

        m_track.getInfo( this, "write", User().name() );
    }

    hideScrobbledNowPlaying();
}

void
ActivityListWidget::onResumed()
{
    m_trackItem->setHidden( false );

    hideScrobbledNowPlaying();
}

void
ActivityListWidget::onPaused()
{
    m_trackItem->setHidden( true );

    hideScrobbledNowPlaying();
}

void
ActivityListWidget::onStopped()
{
    m_trackItem->setHidden( true );

    hideScrobbledNowPlaying();
}

void
ActivityListWidget::hideScrobbledNowPlaying()
{
    for ( int i = 0 ; i < count() ; ++i )
    {
        if ( item( i ) != m_trackItem )
        {
            TrackWidget* trackWidget = static_cast<TrackWidget*>( itemWidget( item( i ) ) );

            item( i )->setHidden( !m_trackItem->isHidden()
                                    && ( trackWidget->track().timestamp().toTime_t() == m_track.timestamp().toTime_t()
                                    || trackWidget->track().timestamp().toTime_t() == ScrobbleService::instance().currentTrack().timestamp().toTime_t() ) );
        }
    }
}

void
ActivityListWidget::refresh()
{
    if ( !m_recentTrackReply )
    {
        m_recentTrackReply = User().getRecentTracks( kScrobbleLimit, 1 );
        connect( m_recentTrackReply, SIGNAL(finished()), SLOT(onGotRecentTracks()) );
        emit refreshing( true );
    }
}

void
ActivityListWidget::onGotRecentTracks()
{
    XmlQuery lfm;

    if ( lfm.parse( qobject_cast<QNetworkReply*>(sender())->readAll() ) )
    {
        m_trackItem->setHidden( true );

        QList<lastfm::Track> tracks;

        foreach ( const XmlQuery& trackXml, lfm["recenttracks"].children("track") )
        {
            if ( trackXml.attribute( "nowplaying" ) == "true" )
            {
                MutableTrack track;
                track.setTitle( trackXml["name"].text() );
                track.setArtist( trackXml["artist"].text() );
                track.setAlbum( trackXml["album"].text() );

                if ( track != m_track )
                {
                    // This is a different track so change to it
                    track.setTimeStamp( QDateTime::fromTime_t( trackXml["date"].attribute("uts").toUInt() ) );

                    track.setImageUrl( lastfm::Small, trackXml["image size=small"].text() );
                    track.setImageUrl( lastfm::Medium, trackXml["image size=medium"].text() );
                    track.setImageUrl( lastfm::Large, trackXml["image size=large"].text() );
                    track.setImageUrl( lastfm::ExtraLarge, trackXml["image size=extralarge"].text() );

                    m_track = track;
                    m_trackItem->setTrack( m_track );

                    connect( m_track.signalProxy(), SIGNAL(loveToggled(bool)), SLOT(write()));

                    m_track.getInfo( this, "write", User().name() );
                }

                m_trackItem->setHidden( false );
            }
            else
            {
                MutableTrack track;
                track.setTitle( trackXml["name"].text() );
                track.setArtist( trackXml["artist"].text() );
                track.setAlbum( trackXml["album"].text() );
                track.setTimeStamp( QDateTime::fromTime_t( trackXml["date"].attribute("uts").toUInt() ) );

                track.setImageUrl( lastfm::Small, trackXml["image size=small"].text() );
                track.setImageUrl( lastfm::Medium, trackXml["image size=medium"].text() );
                track.setImageUrl( lastfm::Large, trackXml["image size=large"].text() );
                track.setImageUrl( lastfm::ExtraLarge, trackXml["image size=extralarge"].text() );

                tracks << track;
            }
        }

        QList<lastfm::Track> addedTracks = addTracks( tracks );

        // This was a track fetched from user.getRecentTracks so we need to find
        // out if it was loved. We can remove this when loved is included there.
        foreach ( const lastfm::Track& addedTrack, addedTracks )
            addedTrack.getInfo(  this, "write", User().name() );

        write();
    }

    emit refreshing( false );

    m_recentTrackReply->deleteLater();

    hideScrobbledNowPlaying();
}


void
ActivityListWidget::onScrobblesSubmitted( const QList<lastfm::Track>& tracks )
{
    // We need to find out if info has already been fetched for this track or not.
    // If the now playing view wasn't visible it won't have been.
    // Also, should also only fetch if the scrobbles list is visible too

    QList<lastfm::Track> addedTracks = addTracks( tracks );

    // Make sure we fetch info for any tracks with unkown loved status
    foreach ( const lastfm::Track& addedTrack, addedTracks )
        if ( addedTrack.loveStatus() == lastfm::Unknown )
            addedTrack.getInfo(  this, "write", User().name() );
}


QList<lastfm::Track>
ActivityListWidget::addTracks( const QList<lastfm::Track>& tracks )
{
    QList<lastfm::Track> addedTracks;

    for ( int i = 0 ; i < tracks.count() ; ++i )
    {
        int pos = -1;

        // start from 1 because 0 is the now playing track
        for ( int j = 0 ; j < count() ; ++j )
        {
            if ( !static_cast<ActivityListWidgetItem*>( item( j ) )->isNowPlaying()
                    && tracks[i].timestamp().toTime_t() == static_cast<TrackWidget*>( itemWidget( item( j ) ) )->track().timestamp().toTime_t() )
            {
                pos = j;
                break;
            }
        }

        if ( pos == -1 )
        {
            // the track was not in the list
            ActivityListWidgetItem* item = new ActivityListWidgetItem( this );
            Track track = tracks[i];
            TrackWidget* trackWidget = new TrackWidget( track, this );
            setItemWidget( item, trackWidget );
            item->setSizeHint( trackWidget->sizeHint() );

            connect( track.signalProxy(), SIGNAL(loveToggled(bool)), SLOT(write()));
            connect( track.signalProxy(), SIGNAL(scrobbleStatusChanged()), SLOT(write()));
        }
        else
        {
            // update the track in the list with the new infos!
        }
    }

    limit( kScrobbleLimit );

    write();

    hideScrobbledNowPlaying();

    return addedTracks;
}


void
ActivityListWidget::limit( int limit )
{
    sortItems();

    if ( count() > limit )
    {
        while ( count() > limit )
        {
            QListWidgetItem* item = takeItem( count() - 1 );
            itemWidget( item )->deleteLater();
            delete item;
        }

        write();
    }
}


//bool
//ActivityListWidget::isNowPlaying() const
//{
//    // if the top track in the list is the now playing track then just use the list
//    if ( m_tracks.count() > 0 && m_tracks[0].timestamp() == m_nowScrobblingTrack.timestamp() )
//        return false;

//    return !m_nowPlayingTrack.isNull() || (!m_nowScrobblingTrack.isNull() && !m_paused);
//}

//QModelIndex
//ActivityListModel::adjustedIndex( const QModelIndex& a_index ) const
//{
//    if ( isNowPlaying() )
//        return createIndex( a_index.row() - 1, a_index.column() );

//    return a_index;
//}


//const ImageTrack&
//ActivityListWidget::indexedTrack( const QModelIndex& index, const QModelIndex& adjustedIndex ) const
//{
//    if ( isNowPlaying() && index.row() == 0 )
//    {
//        // prefer the now scrobbling track if it is there
//        if ( !m_nowScrobblingTrack.isNull() )
//            return m_nowScrobblingTrack;
//        else
//            return m_nowPlayingTrack;
//    }

//    return isNowPlaying() && index.row() == 0 ? m_nowPlayingTrack : m_tracks[adjustedIndex.row()];
//}


//void
//ActivityListWidget::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
//{
//    Track track = index.data( ActivityListModel::TrackRole ).value<Track>();
//    QDateTime timestamp = track.timestamp();
//    bool isNowPlaying = timestamp.toTime_t() == 0 || track.sameObject( ScrobbleService::instance().currentTrack() );

//    const QImage& image = index.data( Qt::DecorationRole ).value<QImage>();
//    QLinearGradient g( option.rect.topLeft(), option.rect.bottomLeft());
//    g.setColorAt( 0, isNowPlaying ? QColor( 0xFFFCCA ) : QColor( 0xeeeeee ) );
//    g.setColorAt( 1, isNowPlaying ? QColor( 0xeeebb9 ) : QColor( 0xdddddd ) );
//    painter->fillRect( option.rect, g );

//    painter->setPen(QColor(0xaaaaaa));
//    painter->drawLine( option.rect.bottomLeft(), option.rect.bottomRight());

//    painter->drawImage( option.rect.topLeft() + QPoint( 20, 10 ), image );

//    QFont f;
//    f.setBold( true );
//    f.setPixelSize( 13 );
//    painter->setPen( QColor(0x333333) );
//    painter->setFont( f );

//    QFontMetrics fm( f );

//    painter->setPen( QColor(0x333333) );
//    QString trackTitle = fm.elidedText( index.data( ActivityListModel::TrackNameRole ).toString(), Qt::ElideRight, option.rect.width() - 94 );
//    painter->drawText( option.rect.left() + 94, option.rect.top() + 23, trackTitle );
//    f.setBold( false );
//    painter->setFont( f );
//    painter->setPen( QColor(0x333333) );
//    QString artist = fm.elidedText( index.data( ActivityListModel::ArtistNameRole ).toString(), Qt::ElideRight, option.rect.width() - 94 );
//    painter->drawText( option.rect.left() + 94, option.rect.top() + 43, artist );

//    painter->setPen( QColor(0x777777) );

//    QString timestampString = isNowPlaying ? tr( "Now playing" ) : unicorn::Label::prettyTime( timestamp );

//    f.setPixelSize( 11 );
//    painter->setFont( f );
//    painter->drawText( option.rect.left() + 94, option.rect.bottom() - 12, timestampString );
//}

