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

#include "lib/unicorn/StylableWidget.h"


class ProgressBar : public StylableWidget
{
    Q_OBJECT

    Q_PROPERTY( QBrush chunk READ chunk WRITE setChunk )

public:
    ProgressBar( QWidget* parent = 0 );

    void setTrack( const Track& track );

    QBrush chunk() const { return m_chunk; }
    void setChunk( const QBrush& chunk ) { m_chunk = chunk; }

public slots:
    void onFrameChanged( int frame );
    
private:
    void resizeEvent( QResizeEvent* e );
    void paintEvent( QPaintEvent* e );

private:
    QImage m_scrobbleMarkerOn;
    QImage m_scrobbleMarkerOff;

    int m_frame;

    Track m_track;

    QBrush m_chunk;
};
