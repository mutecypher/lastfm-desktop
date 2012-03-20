#include <QApplication>
#include <QHeaderView>

#include <lastfm/Track.h>

#include "lib/unicorn/dialogs/ShareDialog.h"
#include "lib/unicorn/dialogs/TagDialog.h"
#include "lib/unicorn/DesktopServices.h"

#include "../Services/ScrobbleService.h"
#include "../Application.h"

#include "ActivityListWidget.h"
#include "MetadataWidget.h"

ActivityListWidget::ActivityListWidget( QWidget* parent )
    :QListWidget( parent )
{
    setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );

    connect( this, SIGNAL( clicked(QModelIndex) ), SLOT(onItemClicked(QModelIndex)));

    setAttribute( Qt::WA_MacNoClickThrough );
}

void 
ActivityListWidget::onItemClicked( const QModelIndex& index ) 
{
    if( index.column() == 1 && index.data( ActivityListModel::HoverStateRole ).toBool() )
    {
        // do nothing here, loving is done in the ActivityListModel
    }
    else if( index.column() == 2 && index.data( ActivityListModel::HoverStateRole ).toBool() )
    {
        TagDialog* td = new TagDialog( index.data(ActivityListModel::TrackRole).value<Track>(), window() );
        td->raise(); 
        td->show(); 
        td->activateWindow();
    }
    else if( index.column() == 3 && index.data( ActivityListModel::HoverStateRole ).toBool() )
    {
        m_shareIndex = index;

        QMenu* shareMenu = new QMenu( this );

        shareMenu->addAction( tr( "Share on Last.fm" ), this, SLOT(onShareLastFm()) );
        shareMenu->addAction( tr( "Share on Twitter" ), this, SLOT(onShareTwitter()) );
        shareMenu->addAction( tr( "Share on Facebook" ), this, SLOT(onShareFacebook()) );

        shareMenu->exec( cursor().pos() );

    }
    else if( index.column() == 4 && index.data( ActivityListModel::HoverStateRole ).toBool() )
    {
        // show the buy links please!
        QString country = aApp->currentSession()->userInfo().country();
        qDebug() << country;

        if ( country.compare( "us", Qt::CaseInsensitive ) == 0 )
            country = "united states";
        else if ( country.compare( "de", Qt::CaseInsensitive ) == 0 )
            country = "germany";
        else
            country = "united kingdom";

        connect( index.data(ActivityListModel::TrackRole).value<lastfm::Track>().getBuyLinks( country ), SIGNAL(finished()), SLOT(onGotBuyLinks()));

        m_buyCursor = cursor().pos();
    }
    else
    {
        emit trackClicked( index.data(ActivityListModel::TrackRole).value<Track>());
    }
}

QString
ActivityListWidget::price( const QString& price, const QString& currency ) const
{
    QString returnPrice;

    if ( currency.compare( "eur", Qt::CaseInsensitive ) == 0 )
        returnPrice = QString::fromUtf8( "€%1" ).arg( price );
    else if ( currency.compare( "usd", Qt::CaseInsensitive ) == 0 )
        returnPrice = QString::fromUtf8( "$%1" ).arg( price );
    else if ( currency.compare( "gbp", Qt::CaseInsensitive ) == 0 )
        returnPrice = QString::fromUtf8( "£%1" ).arg( price );
    else
        returnPrice = QString( "%1 %2" ).arg( price, currency );

    return returnPrice;
}

void
ActivityListWidget::onGotBuyLinks()
{
    XmlQuery lfm;

    if ( lfm.parse( qobject_cast<QNetworkReply*>(sender())->readAll() ) )
    {
        bool thingsToBuy = false;

        QMenu* menu = new QMenu( this );

        menu->addAction( tr("Downloads") )->setEnabled( false );

        // USD EUR GBP

        foreach ( const XmlQuery& affiliation, lfm["affiliations"]["downloads"].children( "affiliation" ) )
        {
            bool isSearch = affiliation["isSearch"].text() == "1";

            QAction* buyAction = 0;

            if ( isSearch )
                buyAction = menu->addAction( tr("Search on %1").arg( affiliation["supplierName"].text() ) );
            else
                buyAction = menu->addAction( tr("Buy on %1 %2").arg( affiliation["supplierName"].text(), price( affiliation["price"]["amount"].text(), affiliation["price"]["currency"].text() ) ) );

            buyAction->setData( affiliation["buyLink"].text() );

            thingsToBuy = true;
        }

        menu->addSeparator();
        menu->addAction( tr("Physical") )->setEnabled( false );

        foreach ( const XmlQuery& affiliation, lfm["affiliations"]["physicals"].children( "affiliation" ) )
        {
            bool isSearch = affiliation["isSearch"].text() == "1";

            QAction* buyAction = 0;

            if ( isSearch )
                buyAction = menu->addAction( tr("Search on %1").arg( affiliation["supplierName"].text() ) );
            else
                buyAction = menu->addAction( tr("Buy on %1 %2").arg( affiliation["supplierName"].text(), price( affiliation["price"]["amount"].text(), affiliation["price"]["currency"].text() ) ) );

            buyAction->setData( affiliation["buyLink"].text() );

            thingsToBuy = true;
        }

        connect( menu, SIGNAL(triggered(QAction*)), SLOT(onBuyActionTriggered(QAction*)) );

        menu->exec( m_buyCursor );
    }
    else
    {
        // TODO: what happens when we fail?
        qDebug() << lfm.parseError().message() << lfm.parseError().enumValue();
    }
}

void
ActivityListWidget::onBuyActionTriggered( QAction* buyAction )
{
    unicorn::DesktopServices::openUrl( buyAction->data().toString() );
}

void
ActivityListWidget::onShareLastFm()
{
    ShareDialog* sd = new ShareDialog( m_shareIndex.data(ActivityListModel::TrackRole).value<Track>(), window() );
    sd->raise();
    sd->show();
    sd->activateWindow();
}

void
ActivityListWidget::onShareTwitter()
{
    ShareDialog::shareTwitter( m_shareIndex.data(ActivityListModel::TrackRole).value<Track>() );
}

void
ActivityListWidget::onShareFacebook()
{
    ShareDialog::shareFacebook( m_shareIndex.data(ActivityListModel::TrackRole).value<Track>() );
}


void
ActivityListWidget::refresh()
{
    m_model->refresh();
}


#include <QApplication>
#include <QDebug>

#include <lastfm/XmlQuery.h>

#include "lib/unicorn/UnicornSession.h"

#include "Services/ScrobbleService/ScrobbleService.h"
#include "ActivityListModel.h"

#define kScrobbleLimit 30

ActivityListModel::ActivityListModel()
    :m_noArt( ":/meta_album_no_art.png" ),
     m_nowPlayingTrack( Track() ),
     m_nowScrobblingTrack( Track() ),
     m_paused( false )
{
    m_loveIcon.addFile( ":/scrobbles_love_OFF_REST.png", QSize( 21, 18 ), QIcon::Normal, QIcon::Off );
    m_loveIcon.addFile( ":/meta_love_ON_REST.png", QSize( 21, 18 ), QIcon::Normal, QIcon::On );
    m_loveIcon.addFile( ":/scrobbles_love_OFF_HOVER.png", QSize( 21, 18 ), QIcon::Selected, QIcon::Off );
    m_loveIcon.addFile( ":/meta_love_ON_HOVER.png", QSize( 21, 18), QIcon::Selected, QIcon::On );

    m_tagIcon.addFile( ":/scrobbles_tag_REST.png", QSize( 18, 18 ), QIcon::Normal, QIcon::Off );
    m_tagIcon.addFile( ":/scrobbles_tag_HOVER.png", QSize( 18, 18 ), QIcon::Selected, QIcon::Off );

    m_shareIcon.addFile( ":/scrobbles_share_REST.png", QSize( 21, 18 ), QIcon::Normal, QIcon::Off );
    m_shareIcon.addFile( ":/scrobbles_share_HOVER.png", QSize( 21, 18 ), QIcon::Selected, QIcon::Off );

    m_buyIcon.addFile( ":/scrobbles_buy_REST.png", QSize( 21, 18 ), QIcon::Normal, QIcon::Off );
    m_buyIcon.addFile( ":/scrobbles_buy_HOVER.png", QSize( 21, 18 ), QIcon::Selected, QIcon::Off );

    m_noArt = m_noArt.scaled( 64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation );

    connect( qApp, SIGNAL( sessionChanged( unicorn::Session* )), SLOT(onSessionChanged( unicorn::Session* )));

    connect( &ScrobbleService::instance(), SIGNAL(scrobblesSubmitted(QList<lastfm::Track>)), SLOT(onScrobblesSubmitted(QList<lastfm::Track>) ) );

    connect( &ScrobbleService::instance(), SIGNAL(trackStarted(Track,Track)), SLOT(onTrackStarted(Track,Track)));
    connect( &ScrobbleService::instance(), SIGNAL(paused()), SLOT(onPaused()));
    connect( &ScrobbleService::instance(), SIGNAL(resumed()), SLOT(onResumed()));
    connect( &ScrobbleService::instance(), SIGNAL(stopped()), SLOT(onStopped()));

    onSessionChanged( lastfm::ws::Username );
}

void
ActivityListModel::onFoundIPodScrobbles( const QList<lastfm::Track>& /*tracks*/ )
{
}

void
ActivityListModel::onSessionChanged( unicorn::Session* session )
{
    onSessionChanged( session->userInfo().name() );
}

void
ActivityListModel::onSessionChanged( const QString& username )
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
ActivityListModel::read()
{
    qDebug() << m_path;

    m_tracks.clear();

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

    reset();
}

void
ActivityListModel::write()
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
ActivityListModel::doWrite()
{
    qDebug() << "Writing recent_tracks";

    if ( m_tracks.count() == 0 )
        QFile::remove( m_path );
    else
    {
        QDomDocument xml;
        QDomElement e = xml.createElement( "recent_tracks" );
        e.setAttribute( "product", QCoreApplication::applicationName() );
        e.setAttribute( "version", "2" );

        for ( int i(0) ; i < m_tracks.count() ; ++i )
            e.appendChild( m_tracks[i].toDomElement( xml ) );

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
ActivityListModel::onTrackStarted( const Track& track, const Track& )
{
    // Don't display Spotify here as we don't know if the current user is the one scrobbling
    // If it is the current user it will be fetch by user.getRecentTracks
    if ( track.extra( "playerId" ) != "spt" )
    {
        m_nowScrobblingTrack = track;
        m_nowScrobblingTrack.getInfo( this, "write", User().name() );
        m_nowScrobblingTrack.fetchImage();

        connect( &m_nowScrobblingTrack, SIGNAL(imageUpdated()), SLOT(onTrackLoveToggled()));
        connect( m_nowScrobblingTrack.signalProxy(), SIGNAL(loveToggled(bool)), SLOT(onTrackLoveToggled()));
    }
    else
        m_nowScrobblingTrack = Track();

    m_paused = false;

    reset();
}

void
ActivityListModel::onResumed()
{
    m_paused = false;
    reset();
}

void
ActivityListModel::onPaused()
{
    m_paused = true;
    reset();
}

void
ActivityListModel::onStopped()
{
    m_nowScrobblingTrack = Track();
    reset();
}

void
ActivityListModel::refresh()
{
    if ( !m_recentTrackReply )
    {
        m_recentTrackReply = User().getRecentTracks( kScrobbleLimit, 1 );
        connect( m_recentTrackReply, SIGNAL(finished()), SLOT(onGotRecentTracks()) );
        emit refreshing( true );
    }
}

void
ActivityListModel::onGotRecentTracks()
{
    XmlQuery lfm;

    if ( lfm.parse( qobject_cast<QNetworkReply*>(sender())->readAll() ) )
    {
        m_nowPlayingTrack = Track();

        QList<lastfm::Track> tracks;

        foreach ( const XmlQuery& trackXml, lfm["recenttracks"].children("track") )
        {
            if ( trackXml.attribute( "nowplaying" ) == "true" )
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

                m_nowPlayingTrack = track;
                m_nowPlayingTrack.fetchImage();

                connect( &m_nowPlayingTrack, SIGNAL(imageUpdated()), SLOT(onTrackLoveToggled()));
                connect( m_nowPlayingTrack.signalProxy(), SIGNAL(loveToggled(bool)), SLOT(onTrackLoveToggled()));

                track.getInfo( this, "write", User().name() );
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
        foreach ( const lastfm::Track& addedTrack, tracks )
            addedTrack.getInfo(  this, "write", User().name() );

    }

    emit refreshing( false );

    m_recentTrackReply->deleteLater();
}

bool
lessThan( const ImageTrack& left, const ImageTrack& right )
{
    return left.timestamp().toTime_t() > right.timestamp().toTime_t();
}

void
ActivityListModel::onScrobblesSubmitted( const QList<lastfm::Track>& tracks )
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
ActivityListModel::addTracks( const QList<lastfm::Track>& tracks )
{
    beginResetModel();

    QList<lastfm::Track> addedTracks;

    foreach ( const lastfm::Track& track, tracks )
    {
        if ( qBinaryFind( m_tracks.begin(), m_tracks.end(), track, lessThan ) == m_tracks.end() )
        {
            addedTracks << track;

            QList<ImageTrack>::iterator insert = qLowerBound( m_tracks.begin(), m_tracks.end(), track, lessThan );
            QList<ImageTrack>::iterator inserted = m_tracks.insert( insert, track );

            inserted->fetchImage();

            connect( &(*inserted), SIGNAL(imageUpdated()), SLOT(onTrackLoveToggled()));
            connect( track.signalProxy(), SIGNAL(loveToggled(bool)), SLOT(onTrackLoveToggled()));
        }
    }

    endResetModel();

    write();

    limit( kScrobbleLimit );

    return addedTracks;
}


void
ActivityListModel::limit( int limit )
{
    if ( m_tracks.count() > limit )
    {
        beginRemoveRows( QModelIndex(), limit, m_tracks.count() - 1 );

        while ( m_tracks.count() > limit )
            m_tracks.removeLast();

        endInsertRows();

        write();
    }
}


void
ActivityListModel::onTrackLoveToggled()
{
    //TODO: emit dataChanged signal for the one track
    reset();
}

bool
ActivityListModel::isNowPlaying() const
{
    // if the top track in the list is the now playing track then just use the list
    if ( m_tracks.count() > 0 && m_tracks[0].timestamp() == m_nowScrobblingTrack.timestamp() )
        return false;

    return !m_nowPlayingTrack.isNull() || (!m_nowScrobblingTrack.isNull() && !m_paused);
}

QModelIndex
ActivityListModel::adjustedIndex( const QModelIndex& a_index ) const
{
    if ( isNowPlaying() )
        return createIndex( a_index.row() - 1, a_index.column() );

    return a_index;
}


const ImageTrack&
ActivityListModel::indexedTrack( const QModelIndex& index, const QModelIndex& adjustedIndex ) const
{
    if ( isNowPlaying() && index.row() == 0 )
    {
        // prefer the now scrobbling track if it is there
        if ( !m_nowScrobblingTrack.isNull() )
            return m_nowScrobblingTrack;
        else
            return m_nowPlayingTrack;
    }

    return isNowPlaying() && index.row() == 0 ? m_nowPlayingTrack : m_tracks[adjustedIndex.row()];
}


QVariant
ActivityListModel::data( const QModelIndex& a_index, int role ) const
{
    QModelIndex index = adjustedIndex( a_index );
    const ImageTrack& track = indexedTrack( a_index, index );

    if ( role == TrackRole )
    {
        return track;
    }
    else if ( role == HoverStateRole )
    {
        qDebug() << "HoverIndex: " << m_hoverIndex;
        qDebug() << "This Index: " << index;

        return m_hoverIndex == index;
    }

    if ( index.column() == 1 )
    {
        if( role == Qt::CheckStateRole )
            return track.isLoved() ? Qt::Checked : Qt::Unchecked;
        else if( role == Qt::DecorationRole )
            return m_loveIcon;
        else if( role == Qt::SizeHintRole )
            return m_loveIcon.actualSize( QSize( 21, 18 ));
        else
            return QVariant();
    }

    if( index.column() == 2 ) {
        if( role == Qt::DecorationRole )
            return m_tagIcon;
        else if( role == Qt::SizeHintRole )
            return m_tagIcon.actualSize( QSize( 18, 18 ));
        else if( role == Qt::DisplayRole )
            return "Tag";
        else
            return QVariant();
    }

    if( index.column() == 3 ) {
        if( role == Qt::DecorationRole )
            return m_shareIcon;
        else if( role == Qt::SizeHintRole )
            return m_shareIcon.actualSize( QSize( 21, 18 ));
        else if( role == Qt::DisplayRole )
            return "Share";
        else
            return QVariant();
    }

    if( index.column() == 4 ) {
        if( role == Qt::DecorationRole )
            return m_buyIcon;
        else if( role == Qt::SizeHintRole )
            return m_buyIcon.actualSize( QSize( 21, 18 ));
        else if( role == Qt::DisplayRole )
            return "Buy";
        else
            return QVariant();
    }

    if( index.column() != 0 ) return QVariant();

    switch( role ) {
        case Qt::DisplayRole: return track.toString();
        case Qt::DecorationRole: {
            const QImage& image = track.image();
            if( image.isNull() ) return m_noArt;
            return image;
        }
        case Qt::SizeHintRole: return QSize( 600, 84 );
        case TrackNameRole: return track.title();
        case ArtistNameRole: return track.artist().toString();
        case TimeStampRole: return track.timestamp();
    }
    return QVariant();
}

bool
ActivityListModel::setData( const QModelIndex& a_index, const QVariant& value, int role )
{
    QModelIndex index = adjustedIndex( a_index );
    const ImageTrack& track = indexedTrack( a_index, index );

    if( role == HoverStateRole && value.toBool() )
    {
        emit dataChanged( m_hoverIndex, m_hoverIndex );
        m_hoverIndex = index;
        emit dataChanged( index, index );
        return true;
    }

    if( index.column() == 1 )
    {
        if( value == Qt::Checked )
            MutableTrack( track ).love();
        else if( value == Qt::Unchecked )
            MutableTrack( track ).unlove();

        emit dataChanged( index, index );

        return true;
    }

    //Needed to force repaint on hover for tag/share etc.
    if( index.column() > 0 )
        emit dataChanged( index, index );

    return false;
}

QModelIndex
ActivityListModel::index( int row, int column, const QModelIndex& /*parent*/ ) const
{
    return createIndex( row, column );
}

QModelIndex
ActivityListModel::parent( const QModelIndex& /*index*/ ) const
{
    return QModelIndex();
}

int
ActivityListModel::rowCount( const QModelIndex& parent ) const
{
    return parent.isValid() ? 0 : isNowPlaying() ? m_tracks.length() + 1 : m_tracks.length();
}

int
ActivityListModel::columnCount( const QModelIndex& parent ) const
{
    return parent.isValid() ? 0 : 5;
}

Qt::ItemFlags
ActivityListModel::flags( const QModelIndex& index ) const
{
    if( index.column() == 1 )
        return (Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    return (Qt::ItemIsEnabled | Qt::ItemIsSelectable );
}

QVariant
ActivityListModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if( orientation != Qt::Horizontal || role != Qt::DisplayRole )
        return QVariant();

    switch( section )
    {
        case 0:
            return tr( "Item" );
    }

    return QVariant();
}

void
TrackDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    Track track = index.data( ActivityListModel::TrackRole ).value<Track>();
    QDateTime timestamp = track.timestamp();
    bool isNowPlaying = timestamp.toTime_t() == 0 || track.sameObject( ScrobbleService::instance().currentTrack() );

    const QImage& image = index.data( Qt::DecorationRole ).value<QImage>();
    QLinearGradient g( option.rect.topLeft(), option.rect.bottomLeft());
    g.setColorAt( 0, isNowPlaying ? QColor( 0xFFFCCA ) : QColor( 0xeeeeee ) );
    g.setColorAt( 1, isNowPlaying ? QColor( 0xeeebb9 ) : QColor( 0xdddddd ) );
    painter->fillRect( option.rect, g );

    painter->setPen(QColor(0xaaaaaa));
    painter->drawLine( option.rect.bottomLeft(), option.rect.bottomRight());

    painter->drawImage( option.rect.topLeft() + QPoint( 20, 10 ), image );

    QFont f;
    f.setBold( true );
    f.setPixelSize( 13 );
    painter->setPen( QColor(0x333333) );
    painter->setFont( f );

    QFontMetrics fm( f );

    painter->setPen( QColor(0x333333) );
    QString trackTitle = fm.elidedText( index.data( ActivityListModel::TrackNameRole ).toString(), Qt::ElideRight, option.rect.width() - 94 );
    painter->drawText( option.rect.left() + 94, option.rect.top() + 23, trackTitle );
    f.setBold( false );
    painter->setFont( f );
    painter->setPen( QColor(0x333333) );
    QString artist = fm.elidedText( index.data( ActivityListModel::ArtistNameRole ).toString(), Qt::ElideRight, option.rect.width() - 94 );
    painter->drawText( option.rect.left() + 94, option.rect.top() + 43, artist );

    painter->setPen( QColor(0x777777) );

    QString timestampString = isNowPlaying ? tr( "Now playing" ) : unicorn::Label::prettyTime( timestamp );

    f.setPixelSize( 11 );
    painter->setFont( f );
    painter->drawText( option.rect.left() + 94, option.rect.bottom() - 12, timestampString );
}

QString
CheckableDelegate::prettyTime( const QDateTime& timestamp ) const
{
    QString dateFormat( "d MMM h:mmap" );
    QDateTime now = QDateTime::currentDateTime();
    int secondsAgo = timestamp.secsTo( now );

    if ( secondsAgo < (60 * 60) )
    {
        // Less than an hour ago
        int minutesAgo = ( timestamp.secsTo( now ) / 60 );
        return (minutesAgo == 1 ? tr( "%1 minute ago" ) : tr( "%1 minutes ago" ) ).arg( QString::number( minutesAgo ) );
    }
    else if ( secondsAgo < (60 * 60 * 6) || now.date() == timestamp.date() )
    {
        // Less than 6 hours ago or on the same date
        int hoursAgo = ( timestamp.secsTo( now ) / (60 * 60) );
        return (hoursAgo == 1 ? tr( "%1 hour ago" ) : tr( "%1 hours ago" ) ).arg( QString::number( hoursAgo ) );
    }
    else
    {
        return timestamp.toString( dateFormat );
        // We don't need to set the timer because this date will never change
    }
}
