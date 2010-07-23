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
#include "StopWatch.h"
#include <QTimeLine>


StopWatch::StopWatch( ScrobblePoint timeout, uint elapsed )
    : m_point( timeout )
{    
    m_timeline = new QTimeLine( m_point * 1000, this );
    m_timeline->setFrameRange(0, m_point * 1000);
    m_timeline->setEasingCurve( QEasingCurve::Linear );
    m_timeline->setUpdateInterval( 20 );

    connect( m_timeline, SIGNAL(finished()), SIGNAL(timeout()) );
    connect( m_timeline, SIGNAL(frameChanged(int)), SIGNAL(frameChanged(int)));
}

StopWatch::~StopWatch()
{
    if (!isTimedOut() && (m_point*1000) - elapsed() < 4000)
        emit timeout();
}


void
StopWatch::start() //private
{
    m_timeline->start();
}


void
StopWatch::pause()
{
    m_timeline->stop();
    emit paused( true );
}

bool
StopWatch::isTimedOut() const
{
    return m_timeline->state() == QTimeLine::NotRunning;
}

uint
StopWatch::elapsed() const
{
    return m_timeline->currentFrame();
}


void
StopWatch::resume()
{
    m_timeline->resume();
    emit paused( false );
}

