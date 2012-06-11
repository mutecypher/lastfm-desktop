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
#ifndef TRACK_IMAGE_FETCHER_H
#define TRACK_IMAGE_FETCHER_H

#include <QObject>
#include <lib/DllExportMacro.h>
#include <lastfm/Track.h>


/** @author <max@last.fm>
  * Fetches the album art for an album, via album.getInfo
  */
class UNICORN_DLLEXPORT TrackImageFetcher : public QObject
{
    Q_OBJECT
public:
    TrackImageFetcher( const Track& track, Track::ImageSize size );

    // The order of preference is this
    // * if we know the album then use the album image
    // * if no album image found, use track image which could be a guess at the album
    // * if neither album nor track image found, fallback to an artist image.
    void startAlbum();
    void startTrack();
    void startArtist();

    Track track() const { return m_track; }

private:
    lastfm::Track m_track;

    QUrl url( const QString& root_node );

    void trackGetInfo();
    void artistGetInfo();
    void fail();
    bool downloadImage( QNetworkReply*, const QString& root_node_name );
    
    Album album() const { return m_track.album(); }
    Artist artist() const { return m_track.artist(); }

signals:
    void finished( const class QPixmap& );

private slots:
    void onAlbumGotInfo();
    void onTrackGotInfo(const QByteArray &data);
    void onArtistGotInfo();
    void onAlbumImageDownloaded();
    void onTrackImageDownloaded();
    void onArtistImageDownloaded();

private:
    lastfm::Track::ImageSize m_size;
};

#endif
