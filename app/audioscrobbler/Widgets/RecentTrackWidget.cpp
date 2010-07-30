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

#include <lastfm/ws.h>

RecentTrackWidget::RecentTrackWidget( const Track& track )
    :StylableWidget(), m_track( track )
{
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );

    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );

    layout->addWidget( ui.albumArt = new HttpImageWidget(), 0, Qt::AlignTop );
    ui.albumArt->setObjectName( "art" );
    ui.albumArt->setHref( track.www() );

    // hide this row until the album art image has loaded
    hide();

    QUrl imageUrl = track.imageUrl( lastfm::Small, true);
    if ( !imageUrl.isEmpty() )
    {
        ui.albumArt->loadUrl( track.imageUrl( lastfm::Small, true) );
        connect( ui.albumArt, SIGNAL(loaded()), SLOT(show()));
        connect( ui.albumArt, SIGNAL(loaded()), SIGNAL(loaded()));
    }
    else
    {
        // The track doesn't know where its image is
        // so try to find it from track.getInfo

        track.getInfo( lastfm::ws::Username, lastfm::ws::SessionKey );
        connect( track.signalProxy(), SIGNAL(gotInfo(XmlQuery)), SLOT(onGotInfo(XmlQuery)));
    }

    layout->addWidget( ui.trackText = new QLabel( track.toString() ), 1, Qt::AlignTop );
    ui.trackText->setObjectName( "trackText" );
    ui.trackText->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );

    layout->addWidget( ui.love = new QLabel("love"), 0, Qt::AlignTop );
    ui.love->setObjectName( "love" );
    //ui.love->setHoverText( tr("A loved track") );
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

    onScrobbleStatusChanged();

    connect( track.signalProxy(), SIGNAL(loveToggled(bool)), SLOT(onLoveToggled(bool)));
    connect( track.signalProxy(), SIGNAL(loveToggled(bool)), loveAction, SLOT(setChecked(bool)));

    connect( track.signalProxy(), SIGNAL(scrobbleStatusChanged()), SLOT(onScrobbleStatusChanged()) );
}

void
RecentTrackWidget::onGotInfo( const XmlQuery& )
{
    // The track should now know where its image is
    // if it has one that is

    ui.albumArt->loadUrl( m_track.imageUrl( lastfm::Small, true) );
    connect( ui.albumArt, SIGNAL(loaded()), SLOT(show()));
    connect( ui.albumArt, SIGNAL(loaded()), SIGNAL(loaded()));
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
    QFontMetrics fm( ui.trackText->font() );
    ui.trackText->setText( fm.elidedText ( m_track.toString(), Qt::ElideRight, ui.trackText->width() - 2 ) );
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
        ui.timestamp->setText( "Yesterday " + m_track.timestamp().toString( "h:ssap" ));
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
        m_timestampTimer->start( 60 * 1000 );
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

void RecentTrackWidget::onScrobbleStatusChanged()
{
    setToolTip( tr("") );

    switch ( m_track.scrobbleStatus() )
    {
    case Track::Null:
        setStatus( "null" );
        break;
    case Track::Cached:
        setStatus( "cached" );
        setToolTip( tr("Cached") );
        break;
    case Track::Submitted:
        setStatus( "submitted" );
        break;
    case Track::Error:
        setStatus( "error" );
        break;
    }

    switch ( m_track.scrobbleError() )
    {
    case Track::FilteredArtistName:
        setToolTip( tr("Artist name did not pass filters") );
        break;
    case Track::FilteredTrackName:
        setToolTip( tr("Track name did not pass filters") );
        break;
    case Track::FilteredAlbumName:
        setToolTip( tr("Album name did not pass filters") );
        break;
    case Track::FilteredTimestamp:
        setToolTip( tr("Timestamp did not pass filters") );
        break;
    case Track::ExceededMaxDailyScrobbles:
        setToolTip( tr("Max daily scrobbles exceeded") );
        break;
    case Track::InvalidStreamAuth:
        setToolTip( tr("Stream auth was invalid") );
        break;
    default:
        break;
    }

    ui.trackText->setStyle(QApplication::style());
}
