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
#include <QDebug>
#include <QPixmap>
#include <QStringList>


TrackImageFetcher::TrackImageFetcher( const Track& track, Track::ImageSize size )
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
            connect( album().getInfo(), SIGNAL(finished()), SLOT(onAlbumGotInfo()) );
    }
    else
        startTrack();
}

void
TrackImageFetcher::startTrack()
{
    QUrl imageUrl = url( "track" );

    if ( imageUrl.isValid() )
        connect( lastfm::nam()->get( QNetworkRequest( imageUrl ) ), SIGNAL(finished()), SLOT(onTrackImageDownloaded()) );
    else
        trackGetInfo();
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
TrackImageFetcher::trackGetInfo()
{
    if (!artist().isNull())
        m_track.getInfo( this, "onTrackGotInfo" );
    else
        fail();
}


void
TrackImageFetcher::artistGetInfo()
{
    if (!artist().isNull())
        connect( artist().getInfo(), SIGNAL(finished()), SLOT(onArtistGotInfo()) );
    else
        fail();
}

void
TrackImageFetcher::onAlbumGotInfo()
{
    if (!downloadImage( (QNetworkReply*)sender(), "album" ))
        startTrack();
}

void
TrackImageFetcher::onTrackGotInfo( const QByteArray& data )
{
    XmlQuery lfm;

    if ( lfm.parse( data ) )
    {
        lastfm::MutableTrack track( m_track );
        track.setImageUrl( Track::MegaImage, lfm["track"]["image size=mega"].text() );
        track.setImageUrl( Track::ExtraLargeImage, lfm["track"]["image size=extralarge"].text() );
        track.setImageUrl( Track::LargeImage, lfm["track"]["image size=large"].text() );
        track.setImageUrl( Track::MediumImage, lfm["track"]["image size=medium"].text() );
        track.setImageUrl( Track::SmallImage, lfm["track"]["image size=small"].text() );

        if (!downloadImage( 0, "track" ))
            startArtist();
    }
    else
    {
        qWarning() << lfm.parseError().message();
    }
}

void
TrackImageFetcher::onArtistGotInfo()
{
    if (!downloadImage( (QNetworkReply*)sender(), "artist" ))
        fail();
}

void
TrackImageFetcher::onAlbumImageDownloaded()
{
    QPixmap i;

    if ( i.loadFromData( qobject_cast<QNetworkReply*>(sender())->readAll() ) )
        emit finished( i );
    else
        startTrack();

    sender()->deleteLater(); //always deleteLater from slots connected to sender()
}

void
TrackImageFetcher::onTrackImageDownloaded()
{
    QPixmap i;

    if ( i.loadFromData( qobject_cast<QNetworkReply*>(sender())->readAll() ) )
        emit finished( i );
    else
        startArtist();

    sender()->deleteLater(); //always deleteLater from slots connected to sender()
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
        if ( root_node == "album" )
        {
            m_track.album().setImageUrl( Track::MegaImage, lfm[root_node]["image size=mega"].text() );
            m_track.album().setImageUrl( Track::ExtraLargeImage, lfm[root_node]["image size=extralarge"].text() );
            m_track.album().setImageUrl( Track::LargeImage, lfm[root_node]["image size=large"].text() );
            m_track.album().setImageUrl( Track::MediumImage, lfm[root_node]["image size=medium"].text() );
            m_track.album().setImageUrl( Track::SmallImage, lfm[root_node]["image size=small"].text() );
        }
        else if ( root_node == "artist" )
        {
            m_track.artist().setImageUrl( Track::MegaImage, lfm[root_node]["image size=mega"].text() );
            m_track.artist().setImageUrl( Track::ExtraLargeImage, lfm[root_node]["image size=extralarge"].text() );
            m_track.artist().setImageUrl( Track::LargeImage, lfm[root_node]["image size=large"].text() );
            m_track.artist().setImageUrl( Track::MediumImage, lfm[root_node]["image size=medium"].text() );
            m_track.artist().setImageUrl( Track::SmallImage, lfm[root_node]["image size=small"].text() );
        }
    }
    else
    {
        qWarning() << lfm.parseError().message();
    }

    QUrl imageUrl = url( root_node );

    qWarning() << root_node << imageUrl;

    if ( imageUrl.isValid() )
    {
        QNetworkReply* get = lastfm::nam()->get( QNetworkRequest( imageUrl ) );

        if ( root_node == "album" )
            connect( get, SIGNAL(finished()), SLOT(onAlbumImageDownloaded()) );
        else if ( root_node == "track" )
            connect( get, SIGNAL(finished()), SLOT(onTrackImageDownloaded()) );
        else
            connect( get, SIGNAL(finished()), SLOT(onArtistImageDownloaded()) );

        return true;
    }

    return false;
}

QUrl
TrackImageFetcher::url( const QString& root_node )
{
    QList<Track::ImageSize> sizes;

    switch ( m_size )
    {
        default:
        case Track::MegaImage: sizes << Track::MegaImage;
        case Track::ExtraLargeImage: sizes << Track::ExtraLargeImage;
        case Track::LargeImage: sizes << Track::LargeImage;
        case Track::MediumImage: sizes << Track::MediumImage;
        case Track::SmallImage: sizes << Track::SmallImage;
    }

    QUrl imageUrl;
    Track::ImageSize foundSize;

    foreach ( Track::ImageSize size, sizes )
    {
        QUrl url;

        if ( root_node == "album" )
            url = m_track.album().imageUrl( size, true );
        else if ( root_node == "track" )
            url = m_track.imageUrl( size, true );
        else if ( root_node == "artist" )
            url = m_track.artist().imageUrl( size, true );

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
    //emit finished( QPixmap() );
}
