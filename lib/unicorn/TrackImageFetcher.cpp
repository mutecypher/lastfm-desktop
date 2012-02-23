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
#include "TrackImageFetcher.h"
#include <lastfm/Track.h>
#include <lastfm/ws.h>
#include <lastfm/XmlQuery.h>
#include <QPixmap>
#include <QStringList>


TrackImageFetcher::TrackImageFetcher( const Track& track, lastfm::ImageSize size )
    :m_track( track ),
     m_size( size )
{
}

void
TrackImageFetcher::startAlbum()
{
    if (!album().isNull()) 
    {
        QUrl imageUrl = url( "album" );

        if ( imageUrl.isValid() )
            connect( lastfm::nam()->get( QNetworkRequest( imageUrl ) ), SIGNAL(finished()), SLOT(onAlbumImageDownloaded()) );
        else
        {
            QNetworkReply* reply = album().getInfo();
            connect( reply, SIGNAL(finished()), SLOT(onAlbumGotInfo()) );
        }
    }
    else
        startArtist();
}


void
TrackImageFetcher::startArtist()
{
    QUrl imageUrl = url( "artist" );

    if ( imageUrl.isValid() )
        connect( lastfm::nam()->get( QNetworkRequest( imageUrl ) ), SIGNAL(finished()), SLOT(onArtistImageDownloaded()) );
    else
        artistGetInfo();
}


void
TrackImageFetcher::onAlbumGotInfo()
{
    if (!downloadImage( (QNetworkReply*)sender(), "album" ))
        artistGetInfo();
}


void
TrackImageFetcher::onAlbumImageDownloaded()
{
    QPixmap i;

    if ( i.loadFromData( qobject_cast<QNetworkReply*>(sender())->readAll() ) )
        emit finished( i );
    else
        artistGetInfo();
    
    sender()->deleteLater(); //always deleteLater from slots connected to sender()
}


void
TrackImageFetcher::artistGetInfo()
{
    if (!artist().isNull())
    {
        QNetworkReply* reply = artist().getInfo();
        connect( reply, SIGNAL(finished()), SLOT(onArtistGotInfo()) );
    }
    else
        fail();
}


void
TrackImageFetcher::onArtistGotInfo()
{
    if (!downloadImage( (QNetworkReply*)sender(), "artist" ))
        fail();
}


void
TrackImageFetcher::onArtistImageDownloaded()
{
    QPixmap i;

    if ( i.loadFromData( qobject_cast<QNetworkReply*>(sender())->readAll() ) )
        emit finished( i );
    else
        fail();
    
    sender()->deleteLater(); //always deleteLater from slots connected to sender()
}


bool
TrackImageFetcher::downloadImage( QNetworkReply* reply, const QString& root_node )
{
    XmlQuery lfm;

    if ( lfm.parse( reply->readAll() ) )
    {
        // cache all the sizes
        if ( root_node == "artist" )
        {
            m_track.artist().setImageUrl( lastfm::Mega, lfm[root_node]["image size=mega"].text() );
            m_track.artist().setImageUrl( lastfm::ExtraLarge, lfm[root_node]["image size=extralarge"].text() );
            m_track.artist().setImageUrl( lastfm::Large, lfm[root_node]["image size=large"].text() );
            m_track.artist().setImageUrl( lastfm::Medium, lfm[root_node]["image size=medium"].text() );
            m_track.artist().setImageUrl( lastfm::Small, lfm[root_node]["image size=small"].text() );
        }
        else
        {
            lastfm::MutableTrack track( m_track );
            track.setImageUrl( lastfm::Mega, lfm[root_node]["image size=mega"].text() );
            track.setImageUrl( lastfm::ExtraLarge, lfm[root_node]["image size=extralarge"].text() );
            track.setImageUrl( lastfm::Large, lfm[root_node]["image size=large"].text() );
            track.setImageUrl( lastfm::Medium, lfm[root_node]["image size=medium"].text() );
            track.setImageUrl( lastfm::Small, lfm[root_node]["image size=small"].text() );

        }

        QUrl imageUrl = url( root_node );

        qDebug() << root_node << imageUrl;

        QNetworkReply* get = lastfm::nam()->get( QNetworkRequest( imageUrl ) );

        if ( root_node == "artist" )
            connect( get, SIGNAL(finished()), SLOT(onArtistImageDownloaded()) );
        else
            connect( get, SIGNAL(finished()), SLOT(onAlbumImageDownloaded()) );

        return true;
    }
    else
    {
        qWarning() << lfm.parseError().message();
    }
    return false;
}

QUrl
TrackImageFetcher::url( const QString& root_node )
{
    QList<lastfm::ImageSize> sizes;

    switch ( m_size )
    {
        default:
        case lastfm::Mega: sizes << lastfm::Mega;
        case lastfm::ExtraLarge: sizes << lastfm::ExtraLarge;
        case lastfm::Large: sizes << lastfm::Large;
        case lastfm::Medium: sizes << lastfm::Medium;
        case lastfm::Small: sizes << lastfm::Small;
    }

    QUrl imageUrl;
    lastfm::ImageSize foundSize;

    foreach ( lastfm::ImageSize size, sizes )
    {
        QUrl const url = root_node == "artist" ? m_track.artist().imageUrl( size ) : m_track.imageUrl( size, false );

        // we seem to get a load of album.getInfos where the node exists
        // but the value is "" and "mega" isn't currently used for album images
        if ( url.isValid() )
        {
            imageUrl = url;
            foundSize = size;
            break;
        }
    }

    return imageUrl;
}


void
TrackImageFetcher::fail()
{
    emit finished( QPixmap() );
}
