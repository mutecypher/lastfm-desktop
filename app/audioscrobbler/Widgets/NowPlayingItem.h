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

#ifndef TRACK_WIDGET_H_
#define TRACK_WIDGET_H_

#include <QWidget>
#include <QMovie>
#include <QPointer>

#include <lastfm/Track>

#include "TrackItem.h"
#include "lib/unicorn/StylableWidget.h"

class ScrobbleInfoFetcher;
class ScrobbleInfoWidget;
class StopWatch;

class NowPlayingItem : public TrackItem
{
    Q_OBJECT
public:
    NowPlayingItem( const Track& track );

    void setTrack( const Track& track );

    QWidget* infoWidget() const;

private:
    void paintEvent( QPaintEvent* event );

private slots:
    void onWatchPaused( bool isPaused );
    void onWatchFinished();

    void updateTimestamp();

private:
    QWidget* m_nullInfo;
};


#endif // TRACK_WIDGET_H_

