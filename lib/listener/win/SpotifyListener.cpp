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

#include <lastfm/Track.h>
#include "SpotifyListener.h"
#include "../PlayerConnection.h"
#include <lastfm/misc.h>

class SpotifyConnection : public PlayerConnection
{
public:
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


SpotifyListenerWin::SpotifyListenerWin( QObject* parent )
    :QObject( parent ), m_lastPlayerState( Stopped )
{
    QTimer* timer = new QTimer( this );
    timer->start( 1000 );
    connect( timer, SIGNAL(timeout()), SLOT(loop()));
}

SpotifyListenerWin::~SpotifyListenerWin()
{
    delete m_connection;
}

BOOL CALLBACK
SpotifyListenerWin::callback( HWND hWnd, LPARAM lParam )
{
    SpotifyListenerWin* self = reinterpret_cast<SpotifyListenerWin*>(lParam);

///    Getting the filename of the app would be more robust
    //wchar_t filename[256];
    //GetWindowModuleFileName( hWnd, filename, 256 );

    //if ( QString::fromWCharArray( filename ).contains( "spotify.exe" ) )
    {
        wchar_t title[256];
        GetWindowText( hWnd, title, 256 );

        QString windowTitle = QString::fromWCharArray( title );

        if ( windowTitle.startsWith( "Spotify" ) )
        {
            QRegExp re( QString( "^Spotify - (.+) %1 (.+)").arg( QChar( 0x2013 )  ), Qt::CaseSensitive, QRegExp::RegExp2 );

            // A window that follows the Spotify track playing format takes priority
            if ( re.indexIn( windowTitle ) == 0 )
                self->m_windowTitle = windowTitle;

            if ( self->m_windowTitle.isEmpty() )
                self->m_windowTitle = windowTitle;

        }
    }

    return TRUE;
}

void
SpotifyListenerWin::loop()
{
    m_windowTitle.clear();

    EnumWindows( SpotifyListenerWin::callback, reinterpret_cast<LPARAM>(this) );

    // stopped =
    // paused = Spotify
    // playing = Spotify - Allo, Darlin' – Kiss Your Lips

    State playerState = Stopped;

    if ( m_windowTitle.startsWith( "Spotify" ) )
    {
        if ( !m_connection )
            emit newConnection( m_connection = new SpotifyConnection );

        QRegExp re( QString( "^Spotify - (.+) %1 (.+)").arg( QChar( 0x2013 )  ), Qt::CaseSensitive, QRegExp::RegExp2 );

        playerState = re.indexIn( m_windowTitle ) == 0 ? Playing : Paused;

        if ( m_lastPlayerState != playerState )
        {
            if ( playerState == Stopped )
            {
                m_connection->stop();
                m_lastTrack = Track();
            }
            else if ( playerState == Paused )
            {
                if ( m_lastPlayerState == Playing )
                    m_connection->pause();
            }
            else if ( playerState == Playing )
            {
                MutableTrack t;
                t.setTitle( re.capturedTexts().at( 2 ) );
                t.setArtist( re.capturedTexts().at( 1 ) );
                // we don't know the duration, but we don't display it so just guess
                t.setDuration( 320 );

                if ( m_lastPlayerState == Paused && t == m_lastTrack )
                    m_connection->resume();
                else
                    m_connection->start( t );

                m_lastTrack = t;
            }
        }
        else if ( playerState == Playing )
        {
            // when going from one song to the next we stay in the play state
            MutableTrack t;
            t.setTitle( re.capturedTexts().at( 2 ) );
            t.setArtist( re.capturedTexts().at( 1 ) );
            // we don't know the duration, but we don't display it so just guess
            t.setDuration( 320 );

            if ( t != m_lastTrack )
                m_connection->start( t );

            m_lastTrack = t;
        }
    }
    else
    {
        if ( m_lastPlayerState == Playing || m_lastPlayerState == Paused )
            m_connection->stop();

        delete m_connection;
    }

    m_lastPlayerState = playerState;
}


//void
//SpotifyListener::blah()
//{
//    static AppleScript playerStateScript( "tell application \"Spotify\" to if running then return player state" );
//    QString playerState = playerStateScript.exec();

//    if ( !playerState.isEmpty() )
//    {
//        if ( !m_connection )
//            emit newConnection( m_connection = new SpotifyConnection );

//        static AppleScript titleScript( "tell application \"Spotify\" to return name of current track" );
//        static AppleScript albumScript( "tell application \"Spotify\" to return album of current track" );
//        static AppleScript artistScript( "tell application \"Spotify\" to return artist of current track" );
//        static AppleScript durationScript( "tell application \"Spotify\" to return duration of current track" );

//        if ( playerState == "playing" )
//        {
//            lastfm::MutableTrack t;
//            t.setTitle( titleScript.exec() );
//            t.setAlbum( albumScript.exec() );
//            t.setArtist( artistScript.exec() );
//            t.setDuration( durationScript.exec().toInt() );

//            if ( t != m_lastTrack )
//                m_connection->start( t );

//            m_lastTrack = t;
//        }
//        else if ( m_lastPlayerState != playerState )
//        {
//            if ( playerState == "stopped" )
//            {
//                m_connection->stop();
//                m_lastTrack = Track();
//            }
//            else if ( playerState == "paused" )
//                m_connection->pause();
//            else if ( playerState == "playing" )
//            {
//                lastfm::MutableTrack t;
//                t.setTitle( titleScript.exec() );
//                t.setAlbum( albumScript.exec() );
//                t.setArtist( artistScript.exec() );
//                t.setDuration( durationScript.exec().toInt() );

//                if ( m_lastPlayerState == "paused" && t == m_lastTrack )
//                    m_connection->resume();
//                else
//                    m_connection->start( t );

//                m_lastTrack = t;
//            }
//        }
//    }
//    else
//    {
//        if ( m_lastPlayerState == "playing" || m_lastPlayerState == "paused" )
//            m_connection->stop();

//        delete m_connection;
//    }

//    m_lastPlayerState = playerState;
//}
