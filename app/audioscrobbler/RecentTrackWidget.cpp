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
#include <QToolButton>

#include <lib/unicorn/dialogs/ShareDialog.h>
#include <lib/unicorn/dialogs/TagDialog.h>

#include "RecentTrackWidget.h"

RecentTrackWidget::RecentTrackWidget( Track& track )
    :QWidget(), m_track( track )
{
    QHBoxLayout* layout = new QHBoxLayout( this );

    layout->addWidget( ui.albumArt = new QLabel("art") );
    ui.albumArt->hide();

    layout->addWidget( ui.title = new QLabel( m_track.toString() ) );

    layout->addStretch( 1 );

    layout->addWidget( ui.love = new QLabel("love") );
    ui.love->setObjectName( "love" );
    ui.love->setVisible( track.isLoved() );

    layout->addWidget( ui.cog = new QToolButton() );
    ui.cog->setObjectName( "cog" );

    QMenu* cogMenu = new QMenu( this );
    QAction* loveAction = cogMenu->addAction( QIcon(":/love-rest.png"), "Love", this, SLOT(onLoveClicked()) );
    loveAction->setCheckable( true );
    loveAction->setChecked( track.isLoved() );

    cogMenu->addAction( QIcon(":/tag-rest.png"), "Tag", this, SLOT(onTagClicked()) );
    cogMenu->addAction( QIcon(":/share-rest.png"), "Share", this, SLOT(onShareClicked()) );

    ui.cog->setMenu( cogMenu );

    connect( track.signalProxy(), SIGNAL(loveToggled(bool)), SLOT(onLoveToggled(bool)));
    connect( track.signalProxy(), SIGNAL(loveToggled(bool)), loveAction, SLOT(setChecked(bool)));
}


void
RecentTrackWidget::onLoveClicked()
{
    MutableTrack track = MutableTrack( m_track );

    if ( track.isLoved() )
        track.unlove();
    else
        track.love();
}

void
RecentTrackWidget::onTagClicked()
{
    TagDialog* tagDialog = new TagDialog( m_track, this );
    tagDialog->show();
}


void
RecentTrackWidget::onShareClicked()
{
    ShareDialog* shareDialog = new ShareDialog( m_track, this );
    shareDialog->show();
}


void
RecentTrackWidget::onLoveToggled( bool loved )
{
    ui.love->setVisible( loved );
}
