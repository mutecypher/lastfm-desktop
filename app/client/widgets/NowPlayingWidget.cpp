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

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QImage>
#include <QPixmap>

#include <lastfm/Track>
#include <lastfm/RadioStation>

#include "lib/unicorn/TrackImageFetcher.h"

#include "../Services/RadioService/RadioService.h"
#include "../Services/ScrobbleService/ScrobbleService.h"

#include "NowPlayingWidget.h"
#include "PlaybackControlsWidget.h"

NowPlayingWidget::NowPlayingWidget( QWidget* parent )
    :QWidget( parent )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    layout->addWidget( ui.playbackControls = new PlaybackControlsWidget( this ) );

    /* This stretch should be replaced by the track info widget */
    layout->addStretch();
}
