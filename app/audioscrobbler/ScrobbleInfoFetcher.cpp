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

#include <QList>
#include <QNetworkReply>

#include <lastfm/ws.h>

#include "ScrobbleInfoFetcher.h"

ScrobbleInfoFetcher::ScrobbleInfoFetcher(QObject* parent)
    :QObject(parent)
{
}

void
ScrobbleInfoFetcher::onTrackStarted( const Track& t, const Track& oldTrack )
{
/// close any outstanding replies from the last track
    foreach (QNetworkReply* reply, m_replies) reply->close();
    m_replies.clear();

/// make some requests for the new track


    if( t != oldTrack ) {
        {
            // track.getInfo
            QNetworkReply* reply = t.getInfo( lastfm::ws::Username, lastfm::ws::SessionKey );
            m_replies.append(reply);
            connect(reply, SIGNAL(finished()), SLOT(onTrackGotInfo()));
        }
        {
        // track.getTopFans
            QNetworkReply* reply = t.getTopFans();
            m_replies.append(reply);
            connect(reply, SIGNAL(finished()), SLOT(onTrackGotTopFans()));
        }
        {
            // track.getTags
            QNetworkReply* reply = t.getTags();
            m_replies.append(reply);
            connect(reply, SIGNAL(finished()), SLOT(onTrackGotTags()));
        }
    }

    if( t.artist() != oldTrack.artist() ) {
        {
            // artist.getInfo
            QNetworkReply* reply = t.artist().getInfo( lastfm::ws::Username, lastfm::ws::SessionKey );
            m_replies.append(reply);
            connect(reply, SIGNAL(finished()), SLOT(onArtistGotInfo()));
        }
        {
            // artist.getGetEvents
            QNetworkReply* reply = t.artist().getEvents( 1 );
            m_replies.append(reply);
            connect(reply, SIGNAL(finished()), SLOT(onArtistGotEvents()));
        }
    }
    if( t.album() != oldTrack.album() ) {
        {
            // album.getInfo
            QNetworkReply* reply = t.album().getInfo( lastfm::ws::Username, lastfm::ws::SessionKey );
            m_replies.append(reply);
            connect(reply, SIGNAL(finished()), SLOT(onAlbumGotInfo()));
        }
    }
}


void
ScrobbleInfoFetcher::onTrackGotInfo()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
    bool removed = m_replies.removeOne(reply);

    XmlQuery lfm = reply->readAll();
    emit trackGotInfo(lfm);
    emit trackGotUserloved(lfm["track"]["userloved"].text() == "1");

    if (m_replies.count() == 0)
        emit finished();
}


void
ScrobbleInfoFetcher::onAlbumGotInfo()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
    bool removed = m_replies.removeOne(reply);

    XmlQuery lfm = reply->readAll();
    emit albumGotInfo(lfm);

    if (m_replies.count() == 0)
        emit finished();
}


void
ScrobbleInfoFetcher::onArtistGotInfo()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
    bool removed = m_replies.removeOne(reply);

    XmlQuery lfm = reply->readAll();
    emit artistGotInfo(lfm);

    if (m_replies.count() == 0)
        emit finished();
}


void
ScrobbleInfoFetcher::onArtistGotEvents()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
    bool removed = m_replies.removeOne(reply);

    XmlQuery lfm = reply->readAll();
    emit artistGotEvents(lfm);

    if (m_replies.count() == 0)
        emit finished();
}


void
ScrobbleInfoFetcher::onTrackGotTopFans()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
    bool removed = m_replies.removeOne(reply);

    XmlQuery lfm = reply->readAll();
    emit trackGotTopFans(lfm);

    if (m_replies.count() == 0)
        emit finished();
}


void 
ScrobbleInfoFetcher::onTrackGotTags()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
    bool removed = m_replies.removeOne(reply);

    XmlQuery lfm = reply->readAll();
    emit trackGotTags(lfm);

    if (m_replies.count() == 0)
        emit finished();
}
