/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Jono Cole and Doug Mansell

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
#include <QPushButton>
#include <QToolButton>
#include <QAction>

#include <QShortcut>

#include "ScrobbleControls.h"
#include "../Application.h"

ScrobbleControls::ScrobbleControls()
{
    new QHBoxLayout( this );
    layout()->setContentsMargins( 0, 0, 0, 0 );

    layout()->addWidget(ui.love = new QPushButton(tr("love")));
    ui.love->setObjectName("love");
    ui.love->setCheckable( true );
    ui.love->setToolTip( tr( "Love track" ) );

    connect(ui.love, SIGNAL(clicked(bool)), qApp, SLOT(changeLovedState(bool)));
    connect( ui.love, SIGNAL( toggled( bool ) ), this, SLOT( onLoveChanged( bool ) ) );
    
    layout()->addWidget(ui.tag = new QPushButton(tr("tag")));
    ui.tag->setObjectName("tag");
    ui.tag->setToolTip( tr( "Add tags" ) );

    layout()->addWidget(ui.share = new QPushButton(tr("share")));
    ui.share->setObjectName("share");
    ui.share->setToolTip( tr( "Share" ) );

    new QShortcut( QKeySequence( Qt::CTRL + Qt::Key_S ), ui.share, SLOT( click() ) );
    new QShortcut( QKeySequence( Qt::CTRL + Qt::Key_T ), ui.tag, SLOT( click() ) );
    new QShortcut( QKeySequence( Qt::CTRL + Qt::Key_L ), ui.love, SLOT( toggle() ) );

    connect( aApp, SIGNAL( busLovedStateChanged(bool)), ui.love, SLOT( setChecked(bool)));
    connect( qApp, SIGNAL( lovedStateChanged(bool)), ui.love, SLOT( setChecked(bool)));

    connect( aApp, SIGNAL( trackStarted( Track, Track)), SLOT( onTrackStarted( Track, Track )));
    connect( aApp, SIGNAL( paused() ), SLOT( onPaused() ) );
    connect( aApp, SIGNAL( resumed() ), SLOT( onResumed() ) );
    connect( aApp, SIGNAL( stopped() ), SLOT( onStopped() ) );

    connect( ui.tag, SIGNAL( clicked()), aApp->tagAction(), SLOT( trigger()));
    connect( ui.share, SIGNAL( clicked()), aApp->shareAction(), SLOT( trigger()));

    setEnabled( false );
}

void
ScrobbleControls::onTrackStarted( const Track&, const Track& )
{
    setEnabled( true );
}

void
ScrobbleControls::onPaused()
{
    setEnabled( false );
}

void
ScrobbleControls::onResumed()
{
    setEnabled( true );
}

void
ScrobbleControls::onStopped()
{
    setEnabled( false );
}

void
ScrobbleControls::setEnabled( bool enabled )
{
    ui.love->setEnabled( enabled );
    ui.tag->setEnabled( enabled );
    ui.share->setEnabled( enabled );
}

void
ScrobbleControls::onLoveChanged( bool checked )
{
    if ( checked )
    {
        ui.love->setToolTip( tr( "Unlove track" ) );
    }
    else
    {
        ui.love->setToolTip( tr( "Love track" ) );
    }
}
