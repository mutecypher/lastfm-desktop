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

#include "lib/unicorn/dialogs/ShareDialog.h"
#include "lib/unicorn/dialogs/TagDialog.h"

#include "ScrobbleControls.h"

#include "../Application.h"

ScrobbleControls::ScrobbleControls( const Track& track )
    :m_track( track )
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );

    layout->addStretch( 1 );

    layout->addWidget(ui.love = new QPushButton(tr("love")));
    ui.love->setObjectName("love");
    ui.love->setCheckable( true );
    ui.love->setToolTip( tr( "Love track" ) );

    connect( ui.love, SIGNAL( toggled( bool ) ), this, SLOT( onLoveChanged( bool ) ) );
    
    layout->addWidget(ui.tag = new QPushButton(tr("tag")));
    ui.tag->setObjectName("tag");
    ui.tag->setToolTip( tr( "Add tags" ) );

    layout->addWidget(ui.share = new QPushButton(tr("share")));
    ui.share->setObjectName("share");
    ui.share->setToolTip( tr( "Share" ) );

    layout->addStretch( 1 );

    new QShortcut( QKeySequence( Qt::CTRL + Qt::Key_S ), ui.share, SLOT( click() ) );
    new QShortcut( QKeySequence( Qt::CTRL + Qt::Key_T ), ui.tag, SLOT( click() ) );
    new QShortcut( QKeySequence( Qt::CTRL + Qt::Key_L ), ui.love, SLOT( toggle() ) );

    connect( ui.tag, SIGNAL( clicked()), SLOT( onTag()));
    connect( ui.share, SIGNAL( clicked()), SLOT( onShare()));

    connect( m_track.signalProxy(), SIGNAL(loveToggled(bool)), SLOT(setLoveChecked(bool)));
}


void
ScrobbleControls::setLoveChecked( bool checked )
{
    /// This just changes the state of the love button

    ui.love->setChecked( checked );

    if ( checked )
        ui.love->setToolTip( tr( "Unlove track" ) );
    else
        ui.love->setToolTip( tr( "Love track" ) );
}


void
ScrobbleControls::onLoveChanged( bool checked )
{
    /// This changes the state of the love button and
    /// loves the track on Last.fm

    // change the button state to the new state. Don't worry
    // it'll get changed back if the web service request fails
    setLoveChecked( checked );

    MutableTrack track( m_track );

    if ( checked )
        track.love();
    else
        track.unlove();
}

void 
ScrobbleControls::onShare()
{
    ShareDialog* sd = new ShareDialog( m_track, window() );
    sd->raise();
    sd->show();
    sd->activateWindow();
}

void
ScrobbleControls::onTag()
{
    TagDialog* td = new TagDialog( m_track, window() );
    td->raise();
    td->show();
    td->activateWindow();
}
