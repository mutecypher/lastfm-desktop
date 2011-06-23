/*
   Copyright 2005-2009 Last.fm Ltd.
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

#include <QCoreApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QFile>
#include <QLabel>
#include <QTimer>
#include <QScrollArea>

#include <lastfm/misc.h>
#include <lastfm/User>

#include "lib/unicorn/layouts/AnimatedListLayout.h"
#include "lib/unicorn/widgets/DataBox.h"

#include "../Services/ScrobbleService.h"
#include "ActivityListWidget.h"
#include "TrackItem.h"
#include "IPodScrobbleItem.h"


const int kNumRecentTracks(30);

ActivityListWidget::ActivityListWidget( QString username, QWidget* parent )
    :StylableWidget( parent ), m_rowNum( 0 ), m_currentItem( 0 )
{  
    QLayout* layout = new QVBoxLayout( this );
    layout->setSpacing(0);
    layout->setContentsMargins( 0, 0, 0, 0 );

    QWidget* content = new QWidget(this);
    content->setObjectName( "contents" );

    m_listLayout = new AnimatedListLayout( kNumRecentTracks, content );
    m_listLayout->setSpacing(0);
    m_listLayout->setContentsMargins( 0, 0, 0, 0 );
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );

    m_listLayout->setSpacing( 0 );

    m_scrollArea = new QScrollArea( this );
    m_scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    m_scrollArea->setWidget( content );
    m_scrollArea->setWidgetResizable( true );

    layout->addWidget( m_scrollArea );

    connect( m_listLayout, SIGNAL(moveFinished()), SLOT(onMoveFinished()));

    m_writeTimer = new QTimer( this );
    m_writeTimer->setSingleShot( true );
    
    setUsername( username );

    connect( m_writeTimer, SIGNAL(timeout()), SLOT(doWrite()) );
    connect( &ScrobbleService::instance(), SIGNAL(foundIPodScrobbles(QList<lastfm::Track>)), SLOT(onFoundIPodScrobbles(QList<lastfm::Track>)));
}

void
ActivityListWidget::refreshRecentTracks( User user )
{
    connect( user.getRecentTracks( kNumRecentTracks ), SIGNAL(finished()), SLOT(onGotRecentTracks()) );
}

void
ActivityListWidget::onGotRecentTracks()
{
    lastfm::XmlQuery lfm = static_cast<QNetworkReply*>(sender())->readAll();

    foreach ( const lastfm::XmlQuery& trackElement, lfm["recenttracks"].children("track") )
    {
        if ( trackElement.attribute("nowplaying") != "true" )
        {
            Track track;
            MutableTrack( track ).setArtist( trackElement["artist"]["name"].text() );
            MutableTrack( track ).setAlbum( trackElement["album"].text() );
            MutableTrack( track ).setTitle( trackElement["name"].text() );

            MutableTrack( track ).setTimeStamp( QDateTime::fromTime_t( trackElement["date"].attribute("uts").toUInt() ) );

            insertCachedTrack( track );
        }
    }

    QTimer::singleShot( 60000, this, SLOT(refreshRecentTracks()) );
}

int
ActivityListWidget::count() const
{
    return m_listLayout->count();
}

class ActivityListItem*
ActivityListWidget::itemAt( int index ) const
{
    return static_cast<ActivityListItem*>(m_listLayout->itemAt( index )->widget());
}

QEasingCurve::Type
ActivityListWidget::easingCurve() const
{
    return m_easingCurve;
}


void
ActivityListWidget::setEasingCurve( QEasingCurve::Type easingCurve )
{
    m_easingCurve = easingCurve;
    m_listLayout->setEasingCurve( m_easingCurve );
}


void
ActivityListWidget::disableHover()
{
    setUpdatesEnabled( false );
}

void
ActivityListWidget::enableHover()
{
    setUpdatesEnabled( true );
}

void
ActivityListWidget::onItemChanged()
{
    write();
}


void ActivityListWidget::setUsername( QString username )
{
    if ( !username.isEmpty() )
    {
        QString path = lastfm::dir::runtimeData().filePath( username + "_recent_tracks.xml" );

        if ( m_path != path )
        {
            m_path = path;
            read();
            refreshRecentTracks( User( username ) );
        }
    }
}

void
ActivityListWidget::read()
{
    qDebug() << m_path;

    m_listLayout->setAnimated( false );

    for ( int i(0) ; i < m_listLayout->count() ; ++i )
    {
        QLayoutItem* item = m_listLayout->takeAt( i );
        delete item->widget();
        delete item;
    }

    QFile file( m_path );
    file.open( QFile::Text | QFile::ReadOnly );
    QTextStream stream( &file );
    stream.setCodec( "UTF-8" );

    QDomDocument xml;
    xml.setContent( stream.readAll() );

    for (QDomNode n = xml.documentElement().lastChild(); !n.isNull(); n = n.previousSibling())
    {
        ActivityListItem* item = ActivityListItem::fromElement( n.toElement() );
        insertItem( item );
    }

    m_listLayout->setAnimated( true );
}

void
ActivityListWidget::write() const
{
    m_writeTimer->start( 200 );
}

void
ActivityListWidget::doWrite() const
{
    if ( m_listLayout->count() == 0 )
        QFile::remove( m_path );
    else
    {
        QDomDocument xml;
        QDomElement e = xml.createElement( "recent_tracks" );
        e.setAttribute( "product", QCoreApplication::applicationName() );
        e.setAttribute( "version", "2" );

        for ( int i(0) ; i < m_listLayout->count() ; ++i )
            e.appendChild( itemAt(i)->toDomElement( xml ) );

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
}

void
ActivityListWidget::insertCachedTrack( const Track& track )
{
    ActivityListItem* item = new TrackItem( track );
    insertItem( item );
}

void
ActivityListWidget::clearItemClicked()
{
    if ( m_currentItem )
    {
        m_currentItem->setSelected( false );
        m_currentItem = 0;
    }
}

void
ActivityListWidget::onItemClicked( ActivityListItem* item )
{
    if ( m_currentItem )
        m_currentItem->setSelected( false );

    m_currentItem = item;
    m_currentItem->setSelected( true );
}

void
ActivityListWidget::onMoveFinished()
{
    while ( m_listLayout->count() > kNumRecentTracks )
    {
        QLayoutItem* item = m_listLayout->takeAt( m_listLayout->count() - 1 );
        delete item->widget();
        delete item;
    }

   write();
}

void
ActivityListWidget::onFoundIPodScrobbles( const QList<lastfm::Track>& tracks )
{
    // Add a TrackItem that displays info about the iPod scrobble
    ActivityListItem* item = new IPodScrobbleItem( tracks );
    item->setObjectName("iPodScrobble");
    item->setOdd( m_rowNum++ % 2);

    connect( item, SIGNAL(clicked(ActivityListItem*)), SIGNAL(itemClicked(ActivityListItem*)));
    connect( item, SIGNAL(clicked(ActivityListItem*)), SLOT(onItemClicked(ActivityListItem*)));

    m_listLayout->addWidget( item );

    write();
}


void
ActivityListWidget::onScrobblesCached( const QList<lastfm::Track>& tracks )
{
    foreach ( lastfm::Track track, tracks )
    {
        // Tracks with a deviceId are iPod scrobbles
        // we ignore these at the moment
        if (track.extra("deviceId").isEmpty())
            insertCachedTrack( track );
    }
}

