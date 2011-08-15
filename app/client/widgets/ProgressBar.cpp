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

#include "ProgressBar.h"

#include "../Services/RadioService.h"
#include "../Services/ScrobbleService.h"
#include "../Services/ScrobbleService/StopWatch.h"

#include <lastfm/Track>
#include <QWidget>
#include <QPaintEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>

#include <phonon/MediaObject>


ProgressBar::ProgressBar( QWidget* parent )
    :StylableWidget( parent )
{
    m_scrobbleMarkerOn = QImage(":/scrobble_marker_ON.png");
    m_scrobbleMarkerOff = QImage(":/scrobble_marker_OFF.png");
}


void
ProgressBar::setTrack( const Track& track )
{
    m_track = track;
    m_frame = 0;
}


void
ProgressBar::onFrameChanged( int frame )
{
    m_frame = frame;
    update();
}


void
ProgressBar::resizeEvent( QResizeEvent* e )
{
    // we set theick interval so we are only told
    // when the progress goes to the next pixel
    if ( !m_track.isNull() && RadioService::instance().mediaObject() )
        RadioService::instance().mediaObject()->setTickInterval( ( m_track.duration() * 1000 ) / e->size().width()  );
}

void
ProgressBar::paintEvent( QPaintEvent* e )
{
    StylableWidget::paintEvent( e );

    QPainter p( this );

    StopWatch* sw = ScrobbleService::instance().stopWatch();

    if ( !m_track.isNull() )
    {
        // draw the chunk
        p.setPen( Qt::transparent );
        p.setBrush( m_chunk );
        p.drawRect( rect().adjusted( 0, 0, ((m_frame * width()) / (m_track.duration() * 1000)) - width(), -1) );

        QString format( "m:ss" );

        QTime duration( 0, 0 );
        duration = duration.addMSecs( m_track.duration() * 1000 );
        QTime progress( 0, 0 );
        progress = progress.addMSecs( m_frame );

        QTextOption to;
        to.setAlignment( Qt::AlignVCenter | Qt::AlignLeft );
        QRect timeRect = rect();
        timeRect.adjust( 6, 0, 0, 0 );

        QFont timeFont = font();
        timeFont.setPixelSize( 10 );
        setFont( timeFont );

        p.setPen( QColor( 0x333333 ) );

        if ( m_track.source() == Track::LastFmRadio )
            p.drawText( timeRect, QString( "%1 / %2" ).arg( progress.toString( format ) ,duration.toString( format ) ), to );
        else
            p.drawText( timeRect, QString( "%1" ).arg( progress.toString( format ) ), to );

        uint scrobblePoint = sw->scrobblePoint() * 1000;

        int scrobbleMarker = (scrobblePoint * width()) / ( m_track.duration() * 1000 ) ;

        p.setPen( QColor( 0xbdbdbd ) );
        p.drawLine( QPoint( scrobbleMarker, rect().top() ),
                    QPoint( scrobbleMarker, rect().bottom() - 1 ) );

        p.setPen( QColor( 0xe6e6e6 ) );
        p.drawLine( QPoint( scrobbleMarker + 1, rect().top() ),
                    QPoint( scrobbleMarker + 1, rect().bottom() - 1 ) );

        // draw the as!
        QImage as( m_frame > scrobblePoint ? m_scrobbleMarkerOn : m_scrobbleMarkerOff );
        QPoint asPoint( scrobbleMarker - 25, (rect().height() / 2) - (as.height() / 2) );
        p.drawImage( asPoint, as );
    }
}
        
