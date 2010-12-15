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

#include <QApplication>
#include <QList>
#include <QNetworkReply>

#include <lastfm/ws.h>

#include "ScrobbleInfoFetcher.h"

ScrobbleInfoFetcher::ScrobbleInfoFetcher( const Track& t, QObject* parent )
    :QObject( parent ), m_track( t ), m_numRequests( 0 ), m_started( false )
{
}

void
ScrobbleInfoFetcher::start()
    {
    if ( m_started == false )
    {
        // only fetch everything once
        m_started = true;

        {
            // track.getInfo
            m_track.getInfo();
            connect( m_track.signalProxy(), SIGNAL(gotInfo(XmlQuery)), SLOT(onTrackGotInfo(XmlQuery)) ) ;
            ++m_numRequests;
        }
        {
            // track.getTopFans
            QNetworkReply* reply = m_track.getTopFans();
            m_replies.append(reply);
            connect(reply, SIGNAL(finished()), SLOT(onTrackGotTopFans()));
        }
        {
            // track.getTags
            QNetworkReply* reply = m_track.getTags();
            m_replies.append(reply);
            connect(reply, SIGNAL(finished()), SLOT(onTrackGotTags()));
        }

        {
            // artist.getInfo
            QNetworkReply* reply = m_track.artist().getInfo( lastfm::ws::Username, lastfm::ws::SessionKey );
            m_replies.append(reply);
            connect(reply, SIGNAL(finished()), SLOT(onArtistGotInfo()));
        }
        {
            // artist.getGetEvents
            QNetworkReply* reply = m_track.artist().getEvents( 1 );
            m_replies.append(reply);
            connect(reply, SIGNAL(finished()), SLOT(onArtistGotEvents()));
        }

        {
            // album.getInfo
            QNetworkReply* reply = m_track.album().getInfo( lastfm::ws::Username, lastfm::ws::SessionKey );
            m_replies.append(reply);
            connect(reply, SIGNAL(finished()), SLOT(onAlbumGotInfo()));
        }
    }
}

void
ScrobbleInfoFetcher::isFinished()
{
    if ( m_replies.count() == 0 && m_numRequests == 0 )
        emit finished();
}

void
ScrobbleInfoFetcher::onTrackGotInfo( const XmlQuery& lfm )
{
    emit trackGotInfo( lfm );
    --m_numRequests;
    isFinished();
}


void
ScrobbleInfoFetcher::onAlbumGotInfo()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
    m_replies.removeOne(reply);

    XmlQuery lfm = reply->readAll();
    emit albumGotInfo(lfm);

    isFinished();
}


void
ScrobbleInfoFetcher::onArtistGotInfo()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
    m_replies.removeOne(reply);

    XmlQuery lfm = reply->readAll();
    emit artistGotInfo(lfm);

    isFinished();
}


void
ScrobbleInfoFetcher::onArtistGotEvents()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
    m_replies.removeOne(reply);

    XmlQuery lfm = reply->readAll();
    emit artistGotEvents(lfm);

    isFinished();
}


void
ScrobbleInfoFetcher::onTrackGotTopFans()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
    m_replies.removeOne(reply);

    XmlQuery lfm = reply->readAll();
    emit trackGotTopFans(lfm);

    isFinished();
}


void 
ScrobbleInfoFetcher::onTrackGotTags()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
    m_replies.removeOne(reply);

    XmlQuery lfm = reply->readAll();
    emit trackGotTags(lfm);

    isFinished();
}
