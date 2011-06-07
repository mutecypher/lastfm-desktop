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
#include "lib/unicorn/widgets/Label.h"

#include "../Application.h"
#include "../ScrobbleInfoFetcher.h"
#include "../Services/RadioService.h"
#include "../Services/ScrobbleService.h"

#include "WelcomeWidget.h"
#include "ScrobbleInfoWidget.h"
#include "NowPlayingItem.h"

#include <lastfm/ws.h>

#include "ui_ActivityListItem.h"


NowPlayingItem::NowPlayingItem( const Track& track )
    :TrackItem( track ),
     m_progressColor(),
     m_scrobblePointColor( "blue" ),
     m_progressWidth( 0 ),
     m_lastFrame( 0 )
{
    m_nullInfo = new WelcomeWidget( this );
    m_nullInfo->hide();

    connect( &ScrobbleService::instance(), SIGNAL(paused(bool)), SLOT( onWatchPaused(bool)) );
    connect( &ScrobbleService::instance(), SIGNAL(timeout()), SLOT( onWatchFinished()));
    connect( &ScrobbleService::instance(), SIGNAL(frameChanged(int)), SLOT(onFrameChanged(int)));

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
        return m_nullInfo;

    return TrackItem::infoWidget();
}


void
NowPlayingItem::onWatchPaused( bool isPaused )
{
    if ( !m_track.isNull() )
    {
        if ( isPaused )
        {
            m_statusText = tr( "Paused" );
            resizeEvent( 0 );
            ui->as->setPixmap( QPixmap() );
        }
        else
        {
            if( (ScrobbleService::instance().stopWatch()->elapsed()/1000.0f) / ScrobbleService::instance().stopWatch()->scrobblePoint() >= 1.0f )
                m_statusText = tr( "Track Scrobbled" );
            else
            {
                //Set to the context if it's a radio track
                m_statusText = contextString( m_track );
            }

            resizeEvent( 0 );
            ui->as->setMovie( movie.scrobbler_as );
            ui->as->movie()->start();
        }
        
        update();
    }
    else
    {
        setText( tr("You're not scrobbling any songs right now - check out your recent tracks below") );
        m_statusText = "";
        resizeEvent( 0 );
        ui->as->clear();
    }
    resizeEvent( 0 );
}


void
NowPlayingItem::onWatchFinished()
{
    connect( ui->as->movie(), SIGNAL(loopFinished()), ui->as->movie(), SLOT(stop()));
    //m_statusText = tr( "Track Scrobbled" );
    //ui->status->setText( m_timestampText );
}


void NowPlayingItem::resizeEvent(QResizeEvent *event)
{
    m_progressWidth = 0;
    onFrameChanged( m_lastFrame );
    TrackItem::resizeEvent( event );
}

QRect
NowPlayingItem::updateRect( int progress ) const
{
    QRect rect = ui->detailsFrame->geometry();
    rect.setLeft( m_progressWidth );
    rect.setRight( progress + 3 );
    return rect;
}

void
NowPlayingItem::onFrameChanged( int frame )
{
    m_lastFrame = frame;
    int progress = 0;

    if ( ScrobbleService::instance().stopWatch() ) {
        progress = ( frame * width() ) / ( ScrobbleService::instance().stopWatch()->duration() * 1000.0 );
    }

    if ( progress != m_progressWidth )
    {  
        m_progressRect = updateRect( progress );
        QRect updateRect;
        if( progress > m_progressWidth )
            updateRect = m_progressRect;
        else {
            updateRect = rect();
            m_progressRect = rect();
            m_progressRect.setRight( m_progressWidth );
        }
  
        m_progressWidth = progress + 3;
        update( updateRect );
    }
}

void
NowPlayingItem::paintEvent( QPaintEvent* event )
{
    StylableWidget::paintEvent( event );
    QPainter p( this );
    p.setPen( QColor( Qt::transparent ));
    p.setBrush( m_progressColor );
    QRect actualRect = m_progressRect.adjusted( 0, 0, -3, 0 );
    p.drawRect( actualRect );

    QRect fadeRect = m_progressRect.adjusted( m_progressRect.width() - 3, 0, 0, 0 );
    QLinearGradient semiGradient( fadeRect.left(), 0.0f, fadeRect.right(), 0.0f);
    if( m_progressColor.gradient())
        semiGradient.setColorAt( 0.0, m_progressColor.gradient()->stops().first().second );
    else
        semiGradient.setColorAt( 0.0, m_progressColor.color() );
    semiGradient.setColorAt( 1.0, Qt::transparent );
    p.setBrush( semiGradient );
    
    p.drawRect( fadeRect );

    if ( ScrobbleService::instance().stopWatch() )
    {
        p.setPen( m_scrobblePointColor );

        int scrobblePoint = ( ScrobbleService::instance().stopWatch()->scrobblePoint() * width() ) / ( ScrobbleService::instance().stopWatch()->duration() );

        p.drawLine( QPoint( scrobblePoint, rect().top() ),
                    QPoint( scrobblePoint, rect().bottom()) );
    }
}

void NowPlayingItem::updateTimestamp()
{
    // Do nothing for the now playing track
    // its timestamp label is not used as a timestamp
}

