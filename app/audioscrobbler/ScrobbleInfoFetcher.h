/*
   Copyright 2005-2009 Last.fm Ltd.
      - Primarily authored by Jono Cole and Doug Mansell

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

#include <QObject>

#include <lastfm/Track>
#include <lastfm/XmlQuery>


class ScrobbleInfoFetcher : public QObject
{
    Q_OBJECT
public:
    ScrobbleInfoFetcher( const Track& t, QObject* parent = 0);
    void start();

signals:
    void setTrack(const Track& track);
    void trackGotInfo(const XmlQuery& lfm);
    void albumGotInfo(const XmlQuery& lfm);
    void artistGotInfo(const XmlQuery& lfm);
    void artistGotEvents(const XmlQuery& lfm);
    void trackGotTopFans(const XmlQuery& lfm);
    void trackGotTags(const XmlQuery& lfm);

    void finished();

private slots:
    void onTrackGotInfo( const XmlQuery& lfm );
    void onAlbumGotInfo();
    void onArtistGotInfo();
    void onArtistGotEvents();
    void onTrackGotTopFans();
    void onTrackGotTags();

private:
    void isFinished();

private:
    Track m_track;

    class QList<class QNetworkReply* > m_replies;
    int m_numRequests;
    bool m_started;
};
