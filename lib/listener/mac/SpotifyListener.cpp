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

<<<<<<< HEAD
#include <types/Track.h>
=======
#include <lastfm/Track.h>
>>>>>>> 70b252e9f4dcef74832c647f891cec7ac0d796d9
#include "SpotifyListener.h"
#include "../PlayerConnection.h"
#include <core/misc.h>
#include "lib/unicorn/mac/AppleScript.h"


struct SpotifyConnection : PlayerConnection
{
    SpotifyConnection() : PlayerConnection( "spt", "Spotify" )
    {}

    void start( const Track& t )
    {
        MutableTrack mt( t );
        mt.setSource( Track::Player );
        mt.setExtra( "playerId", id() );
        mt.setExtra( "playerName", name() );
        mt.stamp();
        handleCommand( CommandStart, t );
    }

    void pause() { handleCommand( CommandPause ); }
    void resume() { handleCommand( CommandResume ); }
    void stop() { handleCommand( CommandStop ); }
};


SpotifyListener::SpotifyListener( QObject* parent )
    :QObject( parent )
{
    QTimer* timer = new QTimer( this );
    timer->start( 1000 );
    connect( timer, SIGNAL(timeout()), SLOT(loop()));
}


void
SpotifyListener::loop()
{
    static AppleScript playerStateScript( "tell application \"Spotify\" to if running then return player state" );
    QString playerState = playerStateScript.exec();

    if ( playerState == "playing"
         || playerState == "stopped"
         || playerState == "paused"

         )
    {
        if ( !m_connection )
            emit newConnection( m_connection = new SpotifyConnection );

        static AppleScript titleScript( "tell application \"Spotify\" to return name of current track" );
        static AppleScript albumScript( "tell application \"Spotify\" to return album of current track" );
        static AppleScript artistScript( "tell application \"Spotify\" to return artist of current track" );
        static AppleScript durationScript( "tell application \"Spotify\" to return duration of current track" );

        if ( playerState == "playing" )
        {
            lastfm::MutableTrack t;
            t.setTitle( titleScript.exec() );
            t.setAlbum( albumScript.exec() );
            t.setArtist( artistScript.exec() );
            t.setDuration( durationScript.exec().toInt() );

            if ( t != m_lastTrack )
                m_connection->start( t );

            m_lastTrack = t;
        }
        else if ( m_lastPlayerState != playerState )
        {
            if ( playerState == "stopped" )
            {
                m_connection->stop();
                m_lastTrack = Track();
            }
            else if ( playerState == "paused" )
                m_connection->pause();
            else if ( playerState == "playing" )
            {
                lastfm::MutableTrack t;
                t.setTitle( titleScript.exec() );
                t.setAlbum( albumScript.exec() );
                t.setArtist( artistScript.exec() );
                t.setDuration( durationScript.exec().toInt() );

                if ( m_lastPlayerState == "paused" && t == m_lastTrack )
                    m_connection->resume();
                else
                    m_connection->start( t );

                m_lastTrack = t;
            }
        }
    }
    else
    {
        if ( m_lastPlayerState == "playing" || m_lastPlayerState == "paused" )
            m_connection->stop();

        delete m_connection;
    }

    m_lastPlayerState = playerState;
}
