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

#include "RecentTracksWidget.h"
#include "RecentTrackWidget.h"

RecentTracksWidget::RecentTracksWidget( QWidget* parent )
    :QWidget( parent )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
}

void
RecentTracksWidget::read( QString path )
{
    foreach ( RecentTrackWidget* trackWidget, m_tracks )
        layout()->removeWidget( trackWidget );
    m_tracks.clear();


    QFile file( path );
    file.open( QFile::Text | QFile::ReadOnly );
    QTextStream stream( &file );
    stream.setCodec( "UTF-8" );

    QDomDocument xml;
    xml.setContent( stream.readAll() );

    for (QDomNode n = xml.documentElement().firstChild(); !n.isNull(); n = n.nextSibling())
    {
        // TODO: recognise all the other AbstractType classes
        if (n.nodeName() == "track")
        {
            Track* track = new Track( n.toElement() );
            RecentTrackWidget* item = new RecentTrackWidget( *track );
            //item->loadImage( track->imageUrl( lastfm::Small, true ));
            layout()->addWidget( item );
            m_tracks << item;
        }
    }
}


void
RecentTracksWidget::write( QString path ) const
{
    if (m_tracks.isEmpty())
    {
        QFile::remove( path );
    }
    else
    {
        QDomDocument xml;
        QDomElement e = xml.createElement( "recent_tracks" );
        e.setAttribute( "product", QCoreApplication::applicationName() );
        e.setAttribute( "version", "2" );

        foreach ( RecentTrackWidget* trackWidget, m_tracks )
            e.appendChild( trackWidget->track().toDomElement( xml ) );

        xml.appendChild( e );

        QFile file( path );
        file.open( QIODevice::WriteOnly | QIODevice::Text );

        QTextStream stream( &file );
        stream.setCodec( "UTF-8" );
        stream << "<?xml version='1.0' encoding='utf-8'?>\n";
        stream << xml.toString( 2 );
    }
}

void
RecentTracksWidget::addCachedTrack( const Track& a_track )
{
    MutableTrack( a_track ).setExtra( "scrobbleStatus", "cached" );

    Track* track = new Track;
    *track = a_track;
    RecentTrackWidget* trackWidget = new RecentTrackWidget( *track );
    //item->loadImage( track->imageUrl( lastfm::Small, true ));

    m_tracks.insert( 0, trackWidget );
    static_cast<QBoxLayout*>(layout())->insertWidget( 0, trackWidget );

    if ( m_tracks.count() > 5 )
    {
        layout()->removeWidget( m_tracks[5] );
        m_tracks[5]->deleteLater();
        m_tracks.removeAt( 5 );
    }
}

void
RecentTracksWidget::addScrobbledTrack( const Track& a_track )
{
    MutableTrack( a_track ).setExtra( "scrobbleStatus", "scrobbled" );
}
