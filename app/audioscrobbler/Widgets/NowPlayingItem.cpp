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

#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QTimer>
#include <QToolButton>

#include "lib/listener/PlayerConnection.h"
#include "lib/unicorn/dialogs/ShareDialog.h"
#include "lib/unicorn/dialogs/TagDialog.h"
#include "lib/unicorn/widgets/GhostWidget.h"
#include "lib/unicorn/widgets/HttpImageWidget.h"

#include "../Application.h"
#include "../StopWatch.h"
#include "../ScrobbleInfoFetcher.h"

#include "ProfileWidget.h"
#include "ScrobbleInfoWidget.h"
#include "NowPlayingItem.h"

#include <lastfm/ws.h>


NowPlayingItem::NowPlayingItem( const Track& track )
    :TrackItem( track )
{
    profile = new ProfileWidget();

    connect( aApp, SIGNAL(paused(bool)), SLOT( onWatchPaused(bool)) );
    connect( aApp, SIGNAL(timeout()), SLOT( onWatchFinished()));
    connect( aApp, SIGNAL(frameChanged(int)), SLOT(update()));

    onWatchPaused( false );
}

void
NowPlayingItem::setTrack( const Track& track )
{
    doSetTrack( track );
    onWatchPaused( false );
}

QWidget*
NowPlayingItem::infoWidget() const
{
    if ( m_track == Track() )
        return profile;

    return TrackItem::infoWidget();
}

void
NowPlayingItem::onWatchPaused( bool isPaused )
{
    if ( !m_track.isNull() )
    {
        if ( isPaused )
        {
            ui.timestamp->setText( tr( "%1 paused" ).arg( aApp->currentConnection()->name() ) );
            ui.as->setMovie( movie.scrobbler_paused );
            ui.as->movie()->start();
        }
        else
        {
            ui.timestamp->setText( aApp->currentConnection()->name() );
            ui.as->setMovie( movie.scrobbler_as );
            ui.as->movie()->start();
        }

        update();
    }
    else
    {
        ui.timestamp->clear();
        ui.as->clear();
    }
}


void
NowPlayingItem::onWatchFinished()
{
    connect( ui.as->movie(), SIGNAL(loopFinished()), ui.as->movie(), SLOT(stop()));
    ui.timestamp->setText( tr( "Track Scrobbled" ));
}


void
NowPlayingItem::paintEvent( QPaintEvent* event )
{
    StylableWidget::paintEvent( event );

    if ( aApp->stopWatch() )
    {
        QPainter p( this );
        p.setPen( QColor( Qt::transparent ));
        p.setBrush( QColor( 0, 0, 0, 60 ));

        float percentage = (aApp->stopWatch()->elapsed()/1000.0f) / aApp->stopWatch()->scrobblePoint();
        p.drawRect( 0, 0, width() * percentage , height());
    }
}

void NowPlayingItem::updateTimestamp()
{
    // Do nothing for the now playing track
    // its timestamp label is not used as a timestamp
}

