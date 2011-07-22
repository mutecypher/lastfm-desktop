#include "RadioConnection.h"
#include "RadioService.h"

RadioConnection::RadioConnection( QObject* parent )
    :PlayerConnection( "ass", "Last.fm Radio", parent )
{    
    connect( &RadioService::instance(), SIGNAL(trackSpooled(Track)), SLOT(onTrackSpooled(Track)));
    connect( &RadioService::instance(), SIGNAL(paused()), SIGNAL(paused()));
    connect( &RadioService::instance(), SIGNAL(resumed()), SIGNAL(resumed()));
    connect( &RadioService::instance(), SIGNAL(stopped()), SIGNAL(stopped()));
}


Track
RadioConnection::track() const
{
    return RadioService::instance().currentTrack();
}


State
RadioConnection::state() const
{
    return RadioService::instance().state();
}

void
RadioConnection::onTrackSpooled( const Track& track )
{
    MutableTrack mTrack( track );
    mTrack.setSource( Track::LastFmRadio );
    mTrack.setExtra( "playerId", m_id );
    mTrack.setExtra( "playerName", m_name );
    emit trackStarted( track, Track() );
}
