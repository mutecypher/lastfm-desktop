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
#include <QMouseEvent>
#include <QTimer>
#include <QToolButton>

#include "lib/unicorn/dialogs/ShareDialog.h"
#include "lib/unicorn/dialogs/TagDialog.h"
#include "lib/unicorn/widgets/GhostWidget.h"
#include "lib/unicorn/widgets/HttpImageWidget.h"

#include "RecentTrackWidget.h"

RecentTrackWidget::RecentTrackWidget( Track& track )
    :StylableWidget(), m_track( track )
{
    QHBoxLayout* layout = new QHBoxLayout( this );

    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->addWidget( ui.albumArt = new HttpImageWidget(), 0, Qt::AlignTop );
    ui.albumArt->setObjectName( "art" );
    ui.albumArt->loadUrl( track.imageUrl( lastfm::Small, true) );
    ui.albumArt->setHref( track.www() );

    layout->addWidget( ui.title = new QLabel( track.toString() ), 1, Qt::AlignTop );
    ui.title->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );

    layout->addWidget( ui.love = new QLabel("love"), 0, Qt::AlignTop );
    ui.love->setObjectName( "love" );
    ui.love->setVisible( track.isLoved() );

    layout->addWidget( ui.cog = new QToolButton(), 0, Qt::AlignTop );
    ui.cog->setObjectName( "cog" );
    layout->addWidget( ui.ghostCog = new GhostWidget( ui.cog ) );
    ui.cog->hide();
    ui.ghostCog->show();

    QMenu* cogMenu = new QMenu( this );
    QAction* loveAction = cogMenu->addAction( QIcon(":/love-rest.png"), "Love", this, SLOT(onLoveClicked()) );
    loveAction->setCheckable( true );
    loveAction->setChecked( track.isLoved() );

    cogMenu->addAction( QIcon(":/tag-rest.png"), "Tag", this, SLOT(onTagClicked()) );
    cogMenu->addAction( QIcon(":/share-rest.png"), "Share", this, SLOT(onShareClicked()) );

    ui.cog->setMenu( cogMenu );

    layout->addWidget( ui.timestamp = new QLabel(), 0, Qt::AlignTop );
    m_timestampTimer = new QTimer( this );
    connect( m_timestampTimer, SIGNAL(timeout()), SLOT(updateTimestamp()));
    updateTimestamp();

    connect( track.signalProxy(), SIGNAL(loveToggled(bool)), SLOT(onLoveToggled(bool)));
    connect( track.signalProxy(), SIGNAL(loveToggled(bool)), loveAction, SLOT(setChecked(bool)));
}

void
RecentTrackWidget::enterEvent( class QEvent* )
{
    ui.cog->show();
    ui.ghostCog->hide();
}

void
RecentTrackWidget::leaveEvent( class QEvent* )
{
    ui.cog->hide();
    ui.ghostCog->show();
}

void
RecentTrackWidget::resizeEvent(QResizeEvent* )
{
    QFontMetrics fm( ui.title->font() );
    ui.title->setText( fm.elidedText ( m_track.toString(), Qt::ElideRight, ui.title->width() - 2 ) );
}

void
RecentTrackWidget::updateTimestamp()
{
    QDateTime now = QDateTime::currentDateTime();

    if ( m_track.timestamp().daysTo( now ) > 1 )
    {
        ui.timestamp->setText(m_track.timestamp().toString( "ddd h:ssap" ));
        m_timestampTimer->start( 24 * 60 * 60 * 1000 );
    }
    else if ( m_track.timestamp().daysTo( now ) == 1 )
    {
        ui.timestamp->setText(m_track.timestamp().toString( "Yesterday h:ssap" ));
        m_timestampTimer->start( 24 * 60 * 60 * 1000 );
    }
    else if ( (m_track.timestamp().secsTo( now ) / (60 * 60) ) > 1 )
    {
        ui.timestamp->setText( QString::number( (m_track.timestamp().secsTo( now ) / (60 * 60) ) ) + " hours ago" );
        m_timestampTimer->start( 60 * 60 * 1000 );
    }
    else if ( (m_track.timestamp().secsTo( now ) / (60 * 60) ) == 1 )
    {
        ui.timestamp->setText( "1 hour ago" );
        m_timestampTimer->start( 60 * 60 * 1000 );
    }
    else if ( (m_track.timestamp().secsTo( now ) / 60 ) == 1 )
    {
        ui.timestamp->setText( "1 minute ago" );
        m_timestampTimer->start( 60 * 60 * 1000 );
    }
    else
    {
        ui.timestamp->setText( QString::number( (m_track.timestamp().secsTo( now ) / 60 ) ) + " minutes ago" );
        m_timestampTimer->start( 60 * 1000 );
    }
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
