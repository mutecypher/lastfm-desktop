
#include <QApplication>
#include <QDebug>

#include <lastfm/XmlQuery>

#include "lib/unicorn/UnicornSession.h"

#include "Services/ScrobbleService/ScrobbleService.h"
#include "ActivityListModel.h"


ActivityListModel::ActivityListModel()
    :m_noArt( ":/noArt.png" )
{
    m_loveIcon.addFile( ":/scrobbles_love_OFF_REST.png", QSize( 21, 18 ), QIcon::Normal, QIcon::Off );
    m_loveIcon.addFile( ":/meta_love_ON_REST.png", QSize( 21, 18 ), QIcon::Normal, QIcon::On );
    m_loveIcon.addFile( ":/scrobbles_love_OFF_HOVER.png", QSize( 21, 18 ), QIcon::Selected, QIcon::Off );
    m_loveIcon.addFile( ":/meta_love_ON_HOVER.png", QSize( 21, 18), QIcon::Selected, QIcon::On );

    m_tagIcon.addFile( ":/scrobbles_tag_REST.png", QSize( 18, 18 ), QIcon::Normal, QIcon::Off );
    m_tagIcon.addFile( ":/scrobbles_tag_HOVER.png", QSize( 18, 18 ), QIcon::Selected, QIcon::Off );

    m_shareIcon.addFile( ":/scrobbles_share_REST.png", QSize( 21, 18 ), QIcon::Normal, QIcon::Off );
    m_shareIcon.addFile( ":/scrobbles_share_HOVER.png", QSize( 21, 18 ), QIcon::Selected, QIcon::Off );

    m_noArt = m_noArt.scaled( 64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation );

    connect( qApp, SIGNAL( sessionChanged( unicorn::Session* )), SLOT(onSessionChanged( unicorn::Session* )));

    connect( &ScrobbleService::instance(), SIGNAL(scrobblesCached(QList<lastfm::Track>)), SLOT(addTracks(QList<lastfm::Track>) ) );
}

void
ActivityListModel::onFoundIPodScrobbles( const QList<lastfm::Track>& tracks )
{
    // Add a TrackItem that displays info about the iPod scrobble
    /*ActivityListItem* item = new IPodScrobbleItem( tracks );
    item->setObjectName("iPodScrobble");
    item->setOdd( m_rowNum++ % 2);

    connect( item, SIGNAL(clicked(ActivityListItem*)), SIGNAL(itemClicked(ActivityListItem*)));
    connect( item, SIGNAL(clicked(ActivityListItem*)), SLOT(onItemClicked(ActivityListItem*)));

    m_listLayout->addWidget( item );

    write();*/
}

void 
ActivityListModel::onSessionChanged( unicorn::Session* session )
{
    const QString& username = session->userInfo().name();

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
    {
        if( n.toElement().attributes().contains( "iPodScrobble" ))
            continue;

        tracks << Track( n.toElement() );
    }

    addTracks( tracks );

    limit( 30 );

    reset();
}

void
ActivityListModel::write() const
{
    qDebug() << "Writing recent_tracks";
    if ( rowCount() == 0 )
        QFile::remove( m_path );
    else
    {
        QDomDocument xml;
        QDomElement e = xml.createElement( "recent_tracks" );
        e.setAttribute( "product", QCoreApplication::applicationName() );
        e.setAttribute( "version", "2" );

        for ( int i(0) ; i < rowCount() ; ++i )
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
ActivityListModel::refresh()
{
    connect( User().getRecentTracks( 30, 1 ), SIGNAL(finished()), SLOT(onGotRecentTracks()) );
    emit refreshing( true );
}

void
ActivityListModel::onGotRecentTracks()
{
    try
    {
        XmlQuery lfm = qobject_cast<QNetworkReply*>(sender())->readAll();

        qDebug() << lfm;

        QList<lastfm::Track> tracks;

        foreach ( const XmlQuery& trackXml, lfm["recenttracks"].children("track") )
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

        addTracks( tracks );
    }
    catch (...)
    {

    }

    emit refreshing( false );
}

void
ActivityListModel::addTracks( const QList<lastfm::Track>& tracks )
{
    foreach ( const lastfm::Track& track, tracks )
    {
        // Tracks with a deviceId are iPod scrobbles
        // we ignore these at the moment
        if ( track.extra("deviceId").isEmpty() )
        {
            int pos = 0;
            bool duplicate = false;

            for ( int i = 0 ; i < m_tracks.count() ; ++i )
            {
                if ( track.timestamp() == m_tracks[i].timestamp() )
                {
                    // DON'T ADD DUPLICATES!
                    duplicate = true;
                    break;
                }
                if ( track.timestamp() < m_tracks[i].timestamp() )
                    pos = i;
                else
                    break;
            }

            if ( duplicate )
                continue;

            beginInsertRows( QModelIndex(), pos, pos );

            m_tracks.insert( pos, track );
            track.getInfo();
            m_tracks[pos].fetchImage();

            endInsertRows();

            connect( &m_tracks[0], SIGNAL(imageUpdated()), SLOT(onTrackLoveToggled()));
            connect( track.signalProxy(), SIGNAL(loveToggled(bool)), SLOT(onTrackLoveToggled()));
            connect( track.signalProxy(), SIGNAL(gotInfo(QByteArray)), SLOT(write()));

            write();
        }
    }

    limit( 30 );
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

QVariant 
ActivityListModel::data( const QModelIndex& index, int role ) const
{
    if ( role == TrackRole )
    {
        return m_tracks[index.row()];
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
            return m_tracks[index.row()].isLoved() ? Qt::Checked : Qt::Unchecked;
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

    if( index.column() != 0 ) return QVariant();

    switch( role ) {
        case Qt::DisplayRole: return m_tracks[index.row()].toString();
        case Qt::DecorationRole: {
            const QImage& image = m_tracks[index.row()].image();
            if( image.isNull() ) return m_noArt;
            return image;
        }
        case Qt::SizeHintRole: return QSize( 600, 84 );
        case TrackNameRole: return m_tracks[index.row()].title();
        case ArtistNameRole: return m_tracks[index.row()].artist().toString();
        case TimeStampRole: return m_tracks[index.row()].timestamp();
    }
    return QVariant();
}

bool 
ActivityListModel::setData( const QModelIndex& index, const QVariant& value, int role ) 
{
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
            MutableTrack(m_tracks[index.row()]).love();
        else if( value == Qt::Unchecked )
            MutableTrack(m_tracks[index.row()]).unlove();

        emit dataChanged( index, index );

        return true; 
    }

    //Needed to force repaint on hover for tag/share etc.
    if( index.column() > 0 )
        emit dataChanged( index, index );

    return false;
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
