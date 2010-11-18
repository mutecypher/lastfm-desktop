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

#include "lib/unicorn/layouts/AnimatedListLayout.h"
#include "lib/unicorn/widgets/DataBox.h"

#include "../Application.h"
#include "RecentTracksWidget.h"
#include "TrackWidget.h"


const int kNumRecentTracks(20);

RecentTracksWidget::RecentTracksWidget( QString username, QWidget* parent )
    :StylableWidget( parent ), m_rowNum( 0 )
{  
    QLayout* layout = new QVBoxLayout( this );
    layout->setSpacing(0);
    layout->setContentsMargins( 0, 0, 0, 0 );

    QWidget* content = new QWidget(this);
    content->setObjectName( "recentTracks" );

    m_listLayout = new AnimatedListLayout( kNumRecentTracks, content );
    m_listLayout->setSpacing(0);
    m_listLayout->setContentsMargins( 0, 0, 0, 0 );
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );

    m_listLayout->setSpacing( 0 );

    m_scrollArea = new QScrollArea( this );
    m_scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    m_scrollArea->setWidget( content );
    m_scrollArea->setWidgetResizable( true );

    layout->addWidget( m_scrollArea );

    connect( m_listLayout, SIGNAL(moveFinished()), SLOT(onMoveFinished()));

    m_writeTimer = new QTimer( this );
    m_writeTimer->setSingleShot( true );
    
    setUsername( username );

    connect( m_writeTimer, SIGNAL(timeout()), SLOT(doWrite()) );
    connect( aApp, SIGNAL(foundIPodScrobbles(QList<Track>)), SLOT(onFoundIPodScrobbles(QList<Track>)));
}


int RecentTracksWidget::count() const
{
    return m_listLayout->count();
}

class TrackWidget* RecentTracksWidget::trackWidget( int index ) const
{
    return static_cast<TrackWidget*>(m_listLayout->itemAt( index )->widget());
}

QEasingCurve::Type
RecentTracksWidget::easingCurve() const
{
    return m_easingCurve;
}


void
RecentTracksWidget::setEasingCurve( QEasingCurve::Type easingCurve )
{
    m_easingCurve = easingCurve;
    m_listLayout->setEasingCurve( m_easingCurve );
}


void
RecentTracksWidget::disableHover()
{
    setUpdatesEnabled( false );
}

void
RecentTracksWidget::enableHover()
{
    setUpdatesEnabled( true );
}

void
RecentTracksWidget::onTrackChanged()
{
    write();
}


void RecentTracksWidget::setUsername( QString username )
{
    QString path = lastfm::dir::runtimeData().filePath( username + "_recent_tracks.xml" );

    if ( m_path != path )
    {
        m_path = path;
        read();
    }
}

void
RecentTracksWidget::read()
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
        // TODO: recognise all the other AbstractType classes
        if (n.nodeName() == "track")
        {
            if (n.attributes().contains("iPodScrobble"))
            {
                TrackWidget* trackWidget = new TrackWidget();
                trackWidget->setText( n.namedItem( "text" ).toElement().text() );
                trackWidget->setObjectName("iPodScrobble");
                trackWidget->setOdd( m_rowNum++ % 2);

                m_listLayout->addWidget( trackWidget );
            }
            else
            {
                Track* track = new Track( n.toElement() );
                TrackWidget* trackWidget = new TrackWidget( *track );
                addTrackWidget( trackWidget );
            }
        }
    }

    m_listLayout->setAnimated( true );
}

void
RecentTracksWidget::write() const
{
    qDebug() << m_path;

    m_writeTimer->start( 200 );
}

void
RecentTracksWidget::doWrite() const
{
    qDebug() << m_path;

    if ( m_listLayout->count() == 0 )
    {
        QFile::remove( m_path );
    }
    else
    {
        QDomDocument xml;
        QDomElement e = xml.createElement( "recent_tracks" );
        e.setAttribute( "product", QCoreApplication::applicationName() );
        e.setAttribute( "version", "2" );

        for ( int i(0) ; i < m_listLayout->count() ; ++i )
        {
            if ( trackWidget(i)->track() != Track() )
            {
                e.appendChild( trackWidget(i)->track().toDomElement( xml ) );
            }
            else
            {
                // This is not a track so must be an iPod scrobble
                QDomElement item = xml.createElement( "track" );
                item.setAttribute("iPodScrobble", "1");
                QDomElement trackText = xml.createElement( "text" );
                trackText.appendChild( xml.createTextNode( trackWidget(i)->text() ) );
                item.appendChild( trackText );
                e.appendChild( item );
            }
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
RecentTracksWidget::addTrackWidget( TrackWidget* trackWidget )
{
    trackWidget->setObjectName("recentTrack");
    trackWidget->setOdd( m_rowNum++ % 2);
    trackWidget->updateTimestamp();
    m_listLayout->addWidget( trackWidget );

    connect( trackWidget->track().signalProxy(), SIGNAL(loveToggled(bool)), SLOT(onTrackChanged()));
    connect( trackWidget->track().signalProxy(), SIGNAL(scrobbleStatusChanged()), SLOT(onTrackChanged()));
    connect( trackWidget->track().signalProxy(), SIGNAL(corrected(QString)), SLOT(onTrackChanged()));

    connect( trackWidget, SIGNAL(cogMenuAboutToHide()), SLOT(enableHover()));
    connect( trackWidget, SIGNAL(cogMenuAboutToShow()), SLOT(disableHover()));

    write();
}

void
RecentTracksWidget::addCachedTrack( const Track& a_track )
{
    TrackWidget* trackWidget = new TrackWidget( a_track );
    addTrackWidget( trackWidget );
}

void
RecentTracksWidget::onMoveFinished()
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
RecentTracksWidget::onFoundIPodScrobbles( const QList<Track>& tracks )
{
    // Add a TrackWidget that displays info about the iPod scrobble
    TrackWidget* trackWidget = new TrackWidget();
    trackWidget->setFromIPodScrobble( tracks );
    trackWidget->setObjectName("iPodScrobble");
    trackWidget->setOdd( m_rowNum++ % 2);

    m_listLayout->addWidget( trackWidget );

    write();
}


void
RecentTracksWidget::onScrobblesCached( const QList<lastfm::Track>& tracks )
{
    foreach ( lastfm::Track track, tracks )
    {
        // Tracks with a deviceId are iPod scrobbles
        // we ignore these at the moment
        if (track.extra("deviceId").isEmpty())
            addCachedTrack( track );
    }
}

void
RecentTracksWidget::setScrollBar( QScrollBar* scrollbar )
{
    m_scrollArea->setVerticalScrollBar( scrollbar );
}

QScrollBar*
RecentTracksWidget::scrollBar() const
{
    return m_scrollArea->verticalScrollBar();
}

