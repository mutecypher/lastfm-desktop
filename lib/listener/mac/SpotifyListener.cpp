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

#include <lastfm/Track>
#include "SpotifyListener.h"
#include "../PlayerConnection.h"
#include <lastfm/misc.h>
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
    QString playerState = AppleScript( "tell application \"Spotify\" to if running then return player state" ).exec();

    if ( !playerState.isEmpty() )
    {
        if ( !m_connection )
            emit newConnection( m_connection = new SpotifyConnection );

        if ( playerState == "playing" )
        {
            lastfm::MutableTrack t;
            t.setTitle( AppleScript( "tell application \"Spotify\" to return name of current track" ).exec() );
            t.setAlbum( AppleScript( "tell application \"Spotify\" to return album of current track" ).exec() );
            t.setArtist( AppleScript( "tell application \"Spotify\" to return artist of current track" ).exec() );
            t.setDuration( AppleScript( "tell application \"Spotify\" to return duration of current track" ).exec().toInt() );

            if ( t != m_lastTrack )
                m_connection->start( t );
        }
        else if ( m_lastPlayerState != playerState )
        {
            if ( playerState == "stopped" )
                m_connection->stop();
            else if ( playerState == "paused" )
                m_connection->pause();
            else if ( playerState == "playing" )
            {
                lastfm::MutableTrack t;
                t.setTitle( AppleScript( "tell application \"Spotify\" to return name of current track" ).exec() );
                t.setAlbum( AppleScript( "tell application \"Spotify\" to return album of current track" ).exec() );
                t.setArtist( AppleScript( "tell application \"Spotify\" to return artist of current track" ).exec() );
                t.setDuration( AppleScript( "tell application \"Spotify\" to return duration of current track" ).exec().toInt() );

                if ( m_lastPlayerState == "paused" && t == m_lastTrack )
                    m_connection->resume();
                else
                    m_connection->start( t );
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
