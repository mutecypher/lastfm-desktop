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

#include "NowPlayingWidget.h"
#include "PlaybackControlsWidget.h"
#include "MetadataWidget.h"

#include "../Services/ScrobbleService.h"

NowPlayingWidget::NowPlayingWidget(QWidget *parent)
    :QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    layout->addWidget( new PlaybackControlsWidget( this ) );

    layout->addStretch( 1 );

    connect( &ScrobbleService::instance(), SIGNAL(trackStarted(Track,Track)), SLOT(onTrackStarted(Track,Track)) );
    connect( &ScrobbleService::instance(), SIGNAL(stopped()), SLOT(onStopped()) );
}

void
NowPlayingWidget::onTrackStarted( const Track& track, const Track& )
{
    if ( m_metadata )
    {
        layout()->removeWidget( m_metadata );
        delete m_metadata;
    }

    qobject_cast<QVBoxLayout*>(layout())->insertWidget( 1, m_metadata = new MetadataWidget( track, false, this ) );
    m_metadata->setBackButtonVisible( false );
}


void
NowPlayingWidget::onStopped()
{
    if ( m_metadata )
    {
        layout()->removeWidget( m_metadata );
        delete m_metadata;
    }
}
