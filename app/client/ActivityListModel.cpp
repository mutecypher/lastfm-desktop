#include "ActivityListModel.h"
#include "../../../lib/unicorn/UnicornSession.h"
#include <QApplication>

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

    for (QDomNode n = xml.documentElement().lastChild(); !n.isNull(); n = n.previousSibling())
    {
        if( n.toElement().attributes().contains( "iPodScrobble" )) continue;
        m_tracks.prepend( ImageTrack(Track( n.toElement() )));
        m_tracks[0].fetchImage();
        connect( &m_tracks[0], SIGNAL(imageUpdated()), SLOT(onTrackLoveToggled()));
  
    }

    reset();
}

void
ActivityListModel::write() const
{
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

/*
void
ActivityListWidget::insertItem( ActivityListItem* item )
{
    item->setObjectName("recentTrack");
    item->setOdd( m_rowNum++ % 2 );
    item->updateTimestamp();

    int pos = 0;

    // find where it should be depending on the timestamp
    for ( int i = 0 ; i < m_listLayout->count() ; ++i )
    {
        QDateTime itemTimestamp = static_cast<ActivityListItem*>(m_listLayout->itemAt( i )->widget())->timestamp();

        qDebug() << item->timestamp() << itemTimestamp;

        if ( item->timestamp() < itemTimestamp )
        {
            pos = i;
        }
        else if ( item->timestamp() == itemTimestamp )
        {
            pos = -1; // duplicate
            item->deleteLater();
            break;
        }
        else
        {
            break;
        }
    }

    if ( pos >= 0 )
    {
        if ( pos == 0 )
            m_listLayout->addWidget( item );
        else
            m_listLayout->insertWidget( pos, item );

        connect( item, SIGNAL(clicked(ActivityListItem*)), SIGNAL(itemClicked(ActivityListItem*)));
        connect( item, SIGNAL(clicked(ActivityListItem*)), SLOT(onItemClicked(ActivityListItem*)));

        connect( item, SIGNAL(changed()), SLOT(onItemChanged()));

        write();
    }
}*/


void
ActivityListModel::onTrackStarted( const Track& track )
{
    // Tracks with a deviceId are iPod scrobbles
    // we ignore these at the moment
    if (track.extra("deviceId").isEmpty()) {
        beginInsertRows( QModelIndex(), 0, 0 );
        m_tracks.prepend( track );
        track.getInfo();
        m_tracks[0].fetchImage();
        connect( &m_tracks[0], SIGNAL(imageUpdated()), SLOT(onTrackLoveToggled()));
        connect( track.signalProxy(), SIGNAL(loveToggled(bool)), SLOT(onTrackLoveToggled()));
        connect( track.signalProxy(), SIGNAL(gotInfo(XmlQuery)), SLOT(write()));
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
ActivityListModel::data( const QModelIndex& index, int role ) const {
    if( role == TrackRole ) 
    {
        return m_tracks[index.row()];
    }
    else if( role == HoverStateRole ) 
    {
        qDebug() << "HoverIndex: " << hoverIndex;
        qDebug() << "This Index: " << index;
        return hoverIndex == index;
    }

    if( index.column() == 1 ) {
        if( role == Qt::CheckStateRole ) 
            return m_tracks[index.row()].isLoved() ? Qt::Checked : Qt::Unchecked;
        else if( role == Qt::DecorationRole )
            return loveIcon;
        else if( role == Qt::SizeHintRole )
            return loveIcon.actualSize( QSize( 40, 84 ));
        else
            return QVariant();
    }

    if( index.column() == 2 ) {
        if( role == Qt::DecorationRole )
            return tagIcon;
        else if( role == Qt::SizeHintRole )
            return tagIcon.actualSize( QSize( 40, 84 ));
        else
            return QVariant();
    }

    switch( role ) {
        case Qt::DisplayRole: return m_tracks[index.row()].toString();
        case Qt::DecorationRole: return m_tracks[index.row()].image();
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
    if( role == HoverStateRole && value.toBool() ) {
        emit dataChanged( hoverIndex, hoverIndex );
        hoverIndex = index;
        emit dataChanged( index, index );
        return true;
    }

    if( index.column() == 1 ) {
        if( value == Qt::Checked ) {
            MutableTrack(m_tracks[index.row()]).love();
        } else if( value == Qt::Unchecked ) {
            MutableTrack(m_tracks[index.row()]).unlove();
        }
        emit dataChanged( index, index );
        return true; 
    }
    //Needed to force repaint on hover for tag/share etc.
    if( index.column() > 0 ) emit dataChanged( index, index );

    return false;
}

Qt::ItemFlags 
ActivityListModel::flags( const QModelIndex& index ) const {
    if( index.column() == 1 ) {
        return (Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    }
    return (Qt::ItemIsEnabled | Qt::ItemIsSelectable );
}
