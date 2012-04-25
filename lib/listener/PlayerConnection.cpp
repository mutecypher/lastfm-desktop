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
#include "PlayerConnection.h"
#include <QtAlgorithms>
#include <QDebug>
#include <QTimer>


class Error
{   
    const char* m_msg;
protected:
    Error( const char* msg ) : m_msg( msg ) {}
    virtual ~Error() {}
public:
    const char* message() const { return m_msg; }
    
    virtual bool isFatal() const { return false; }
};

struct NonFatalError : public Error
{
    NonFatalError( const char* msg ) : Error( msg ) {}    
};

struct FatalError : public Error
{
    FatalError( const char* msg ) : Error( msg ) {}
    virtual bool isFatal() const { return true; }
};

PlayerConnection::PlayerConnection()
    : m_elapsed( 0 ), m_state( Stopped )
{
}

PlayerConnection::PlayerConnection( const QString& id, const QString& name, QObject* parent )
    :QObject( parent )
    , m_id( id )
    , m_name( name )
    , m_elapsed( 0 )
    , m_state( Stopped )
{
    Q_ASSERT( id.size() );
}


void PlayerConnection::forceTrackStarted( const Track& t )
{
    emit trackStarted( track(), t );
}

void PlayerConnection::forcePaused()
{
    emit paused();
}

void
PlayerConnection::handleCommand( PlayerCommand command, Track t )
{
    qDebug() << command;

    try
    {
        switch (command)
        {
            case CommandStart:
                if (t.isNull()) throw FatalError("Can't start a null track");
                m_state = Playing;
                if ( m_stoppedTimer ) m_stoppedTimer->stop();
                if (t == m_track
                        && t.timestamp() == m_track.timestamp())
                {
                    emit resumed();
                    throw NonFatalError("Already playing this track");
                }
                qSwap(m_track, t);
                m_elapsed = 0;
                emit trackStarted( m_track, t );
                break;
                
            case CommandPause:
                if (m_track.isNull()) throw FatalError("Cannot pause a null track");
                if (m_state == Paused) throw NonFatalError("Already paused");
                m_state = Paused;
                emit paused();
                break;

            case CommandResume:
                if (m_track.isNull()) throw FatalError("Can't resume null track");
                if (m_state == Playing) throw NonFatalError("Already playing");
                m_state = Playing;
                emit resumed();
                break;

            case CommandTerm:
            case CommandInit:
            case CommandStop:
                // don't process the stop straight away because we could be skipping
                // track so wait a second to make sure we don't get a start command
                if ( !m_stoppedTimer )
                {
                    m_stoppedTimer = new QTimer( this );
                    m_stoppedTimer->setSingleShot( true );
                    m_stoppedTimer->setInterval( 1000 );
                    connect( m_stoppedTimer, SIGNAL(timeout()), this, SLOT(onStopped()) );
                }

                m_stoppedTimer->start();
                break;
                
            case CommandBootstrap:
				emit bootstrapReady( id() );
                break;
        }
        
    }
    catch (Error& error)
    {
        qWarning() << error.message();

        if (error.isFatal())
        {
            m_state = Stopped;
            m_track = Track();
            emit stopped();
        }
    }
}

void
PlayerConnection::onStopped()
{
    m_track = Track();

    if (m_state == Stopped)
    {
        qWarning() << "Already stopped";
    }
    else
    {
        m_state = Stopped;
        m_elapsed = 0;
        emit stopped();
    }
}
