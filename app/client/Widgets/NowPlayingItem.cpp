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
#include "Services/RadioService.h"
#include "Services/ScrobbleService.h"

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

    connect( aApp, SIGNAL(paused(bool)), SLOT( onWatchPaused(bool)) );
    connect( aApp, SIGNAL(timeout()), SLOT( onWatchFinished()));
    connect( aApp, SIGNAL(frameChanged(int)), SLOT(onFrameChanged(int)));

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


QString userLibrary( const QString& user, const QString& artist )
{
    return Label::anchor( QString("http://www.last.fm/user/%1/library/music/%2").arg( user, artist ), user );
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
            if( (scrobbleService->stopWatch()->elapsed()/1000.0f) / scrobbleService->stopWatch()->scrobblePoint() >= 1.0f )
                m_statusText = tr( "Track Scrobbled" );
            else
            {
                //Set to the context if it's a radio track
                lastfm::TrackContext context = m_track.context();

                if (context.values().count() > 0)
                {

                    QString contextString;

                    switch ( context.type() )
                    {
                    case lastfm::TrackContext::Artist:
                        {
                        switch ( context.values().count() )
                            {
                            default:
                            case 1: contextString = tr( "because you listen to %1" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ) ); break;
                            case 2: contextString = tr( "because you listen to %1 and %2" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ), Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ) ); break;
                            case 3: contextString = tr( "because you listen to %1, %2, and %3" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ) , Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ), Label::anchor( Artist( context.values().at(2) ).www().toString(), context.values().at(2) ) ); break;
                            case 4: contextString = tr( "because you listen to %1, %2, %3, and %4" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ), Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ), Label::anchor( Artist( context.values().at(2) ).www().toString(), context.values().at(2) ), Label::anchor( Artist( context.values().at(3) ).www().toString(), context.values().at(3) ) ); break;
                            case 5: contextString = tr( "because you listen to %1, %2, %3, %4, and %5" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ), Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ), Label::anchor( Artist( context.values().at(2) ).www().toString(), context.values().at(2) ), Label::anchor( Artist( context.values().at(3) ).www().toString(), context.values().at(3) ), Label::anchor( Artist( context.values().at(4) ).www().toString(), context.values().at(4) ) ); break;
                            }
                        }
                        break;
                    case lastfm::TrackContext::User:
                        // Whitelist multi-user station
                        if ( !radio->station().url().startsWith("lastfm://users/") )
                            break;
                    case lastfm::TrackContext::Friend:
                    case lastfm::TrackContext::Neighbour:
                        {
                        switch ( context.values().count() )
                            {
                            default:
                            case 1: contextString = tr( "from %2%1s library" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), m_track.artist().name() ) ); break;
                            case 2: contextString = tr( "from %2 and %3%1s libraries" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), m_track.artist().name() ), userLibrary( context.values().at(1), m_track.artist().name() ) ); break;
                            case 3: contextString = tr( "from %2, %3, and %4%1s libraries" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), m_track.artist().name() ), userLibrary( context.values().at(1), m_track.artist().name() ), userLibrary( context.values().at(2), m_track.artist().name() ) ); break;
                            case 4: contextString = tr( "from %2, %3, %4, and %5%1s libraries" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), m_track.artist().name() ),userLibrary(  context.values().at(1), m_track.artist().name() ), userLibrary( context.values().at(2), m_track.artist().name() ), userLibrary( context.values().at(3), m_track.artist().name() ) ); break;
                            case 5: contextString = tr( "from %2, %3, %4, %5, and %6%1s libraries" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), m_track.artist().name() ), userLibrary( context.values().at(1), m_track.artist().name() ), userLibrary( context.values().at(2), m_track.artist().name() ), userLibrary( context.values().at(3), m_track.artist().name() ), userLibrary( context.values().at(4), m_track.artist().name() ) ); break;
                            }
                        }
                        break;
                    }

                    m_statusText = contextString;

#ifdef CLIENT_ROOM_RADIO
                    QString strippedContextString = contextString;

                    QRegExp re( "<[^>]*>" );

                    strippedContextString.replace( re, "" );

                    QString ircMessage = QString( "#last.clientradio %1 %2" ).arg( track.toString(), strippedContextString );

                    if ( context.values().count() == ( radio->station().url().count( "," ) + 1 ) )
                        ircMessage.append( " BINGO!" );

                    QTcpSocket socket;
                    socket.connectToHost( "localhost", 12345 );
                    socket.waitForConnected();
                    socket.write( ircMessage.toUtf8() );
                    socket.flush();
                    socket.close();
#endif
                }
                else
                    m_statusText.clear();
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
    onFrameChanged( m_lastFrame );
    TrackItem::resizeEvent( event );
}

QRect
NowPlayingItem::updateRect() const
{
    QRect rect = ui->detailsFrame->geometry();
    rect.setLeft( 0 );
    rect.setRight( m_progressWidth );
    return rect;
}

void
NowPlayingItem::onFrameChanged( int frame )
{
    m_lastFrame = frame;
    int progress = 0;
    if ( scrobbleService->stopWatch() )
        progress = ( frame * width() ) / ( scrobbleService->stopWatch()->duration() * 1000 );

    if ( progress != m_progressWidth )
    {  
        QRect r;
        if( progress > m_progressWidth )
            r = updateRect();
        else
            r = rect();
  
        m_progressWidth = progress;
        update( r );
    }
}

void
NowPlayingItem::paintEvent( QPaintEvent* event )
{
    StylableWidget::paintEvent( event );
    QPainter p( this );
    p.setPen( QColor( Qt::transparent ));
    p.setBrush( m_progressColor );    
    p.drawRect( updateRect() );

    if ( scrobbleService->stopWatch() )
    {
        p.setPen( m_scrobblePointColor );

        int scrobblePoint = ( scrobbleService->stopWatch()->scrobblePoint() * width() ) / ( scrobbleService->stopWatch()->duration() );

        p.drawLine( QPoint( scrobblePoint, updateRect().top() ),
                    QPoint( scrobblePoint, updateRect().bottom()) );
    }
}

void NowPlayingItem::updateTimestamp()
{
    // Do nothing for the now playing track
    // its timestamp label is not used as a timestamp
}

