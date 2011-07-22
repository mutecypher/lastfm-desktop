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

#include <lastfm/Track>
#include <QWidget>
#include <QPaintEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>


ProgressBar::ProgressBar( QWidget* parent )
    :QProgressBar( parent )
{ 
}
        

void
ProgressBar::paintEvent( QPaintEvent* e )
{
    QProgressBar::paintEvent( e );

    QPainter p( this );

    p.drawText( rect(), "Hello" );

    int scrobbleMarker = rect().width() / 2;

    p.setPen( QColor( 0xbdbdbd ) );
    p.drawLine( QPoint( scrobbleMarker, rect().top() ),
                QPoint( scrobbleMarker, rect().bottom() - 1 ) );

    p.setPen( QColor( 0xe6e6e6 ) );
    p.drawLine( QPoint( scrobbleMarker + 1, rect().top() ),
                QPoint( scrobbleMarker + 1, rect().bottom() - 1 ) );


}
        
