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
#include <QTimer>

#include "NowPlayingWidget.h"
#include "PlaybackControlsWidget.h"
#include "MetadataWidget.h"

#include "../Services/RadioService.h"
#include "../Services/ScrobbleService.h"

NowPlayingWidget::NowPlayingWidget(QWidget *parent)
    :QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    layout->addWidget( ui.playbackControls = new PlaybackControlsWidget( this ) );

    layout->addStretch( 1 );

    connect( &RadioService::instance(), SIGNAL(tuningIn(RadioStation)), SLOT(onTuningIn(RadioStation)) );

    connect( &ScrobbleService::instance(), SIGNAL(trackStarted(Track,Track)), SLOT(onTrackStarted(Track,Track)) );
    connect( &ScrobbleService::instance(), SIGNAL(stopped()), SLOT(onStopped()) );
}

PlaybackControlsWidget*
NowPlayingWidget::playbackControls() const
{
    return ui.playbackControls;
}

void
NowPlayingWidget::onTuningIn( const RadioStation& )
{   
    setUpdatesEnabled( false );

    if ( m_metadata )
    {
        layout()->removeWidget( m_metadata );
        m_metadata->deleteLater();

        if ( layout()->count() == 1 )
            qobject_cast<QVBoxLayout*>(layout())->addStretch( 1 );
    }

    setUpdatesEnabled( true );

}

void
NowPlayingWidget::onTrackStarted( const Track& track, const Track& )
{
    setUpdatesEnabled( false );

    if ( m_metadata )
    {
        layout()->removeWidget( m_metadata );
        m_metadata->deleteLater();
    }
    else
    {
        // remove the stretch
        layout()->takeAt( 1 );
    }

    qobject_cast<QVBoxLayout*>(layout())->insertWidget( 1, m_metadata = new MetadataWidget( track, this ), 1 );
    m_metadata->setBackButtonVisible( false );

    setUpdatesEnabled( true );
}

void
NowPlayingWidget::onStopped()
{
    setUpdatesEnabled( false );

    if ( m_metadata )
    {
        layout()->removeWidget( m_metadata );
        m_metadata->deleteLater();

        if ( layout()->count() == 1 )
            qobject_cast<QVBoxLayout*>(layout())->addStretch( 1 );
    }

    setUpdatesEnabled( true );
}
