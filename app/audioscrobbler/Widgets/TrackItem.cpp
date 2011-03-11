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

#include "lib/listener/PlayerConnection.h"
#include "lib/unicorn/dialogs/ShareDialog.h"
#include "lib/unicorn/dialogs/TagDialog.h"
#include "lib/unicorn/widgets/GhostWidget.h"
#include "lib/unicorn/widgets/HttpImageWidget.h"

#include "../Application.h"
#include "../StopWatch.h"
#include "../ScrobbleInfoFetcher.h"

#include "ScrobbleInfoWidget.h"
#include "TrackItem.h"

#include <lastfm/ws.h>


TrackItem::TrackItem( const Track& track )
    :ActivityListItem()
{
    setupUi();
    doSetTrack( track );
}


TrackItem::TrackItem( const TrackItem& that )
{
    setupUi();

    m_track = that.m_track;
    m_fetcher = that.m_fetcher;
    m_infoWidget = that.m_infoWidget;
    m_timestamp = that.m_timestamp;
    m_status = that.m_status;
    setToolTip( that.toolTip() );

    connectTrack();
    setDetails();
}


TrackItem::TrackItem( const QDomElement& element )
    :ActivityListItem()
{
    Track track( element );

    setupUi();
    doSetTrack( track );
}


void
TrackItem::doSetTrack( const Track& track )
{
    /// disconnect the old track
    disconnect( m_track.signalProxy(), 0, this, 0 );

    m_track = track;

    /// Create the fetcher and the info widget for this track
    m_fetcher = new ScrobbleInfoFetcher( track, this );
    m_infoWidget = new ScrobbleInfoWidget( track, m_fetcher, this );
    m_infoWidget->hide();

    connectTrack();
    setDetails();
    onScrobbleStatusChanged();
}

void
TrackItem::setDetails()
{
    /// Set some track specific data for the UI
    ui.love->setVisible( m_track.isLoved() );
    onCorrected( m_track.toString() );
    connectTrack();

    // track items need their timestamp updated as they grow old
    m_timestamp = m_track.timestamp();
    updateTimestamp();
}

void
TrackItem::connectTrack()
{
    connect( m_track.signalProxy(), SIGNAL(corrected(QString)), SLOT(onCorrected(QString)));
    connect( m_track.signalProxy(), SIGNAL(loveToggled(bool)), SLOT(onLoveToggled(bool)));
    connect( m_track.signalProxy(), SIGNAL(scrobbleStatusChanged()), SLOT(onScrobbleStatusChanged()) );

    // make sure we tell people when we change
    connect( m_track.signalProxy(), SIGNAL(loveToggled(bool)), SIGNAL(changed()));
    connect( m_track.signalProxy(), SIGNAL(scrobbleStatusChanged()), SIGNAL(changed()));
    connect( m_track.signalProxy(), SIGNAL(corrected(QString)), SIGNAL(changed()));
}


QDomElement
TrackItem::toDomElement( QDomDocument xml ) const
{
    return m_track.toDomElement( xml );
}


QWidget*
TrackItem::basicInfoWidget() const
{
    return infoWidget();
}


QWidget*
TrackItem::infoWidget() const
{
    // Start the fetcher a bit later so that
    // we switch to the item immediately
    QTimer::singleShot( 0, m_fetcher, SLOT(start()));

    return m_infoWidget;
}


void
TrackItem::onCorrected( QString correction )
{
    if ( m_track.corrected() )
    {
        setText( correction );
        ui.correction->show();
        ui.correction->setToolTip( tr("Auto-corrected from: ") + m_track.toString( lastfm::Track::Original ) );
    }
    else
    {
        setText( m_track.toString() );
        ui.correction->hide();
    }
}


void
TrackItem::onLoveToggled( bool loved )
{
    ui.love->setVisible( loved );
}


void
TrackItem::onScrobbleStatusChanged()
{    setToolTip( tr("") );

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

    ui.text->setStyle(QApplication::style());
}

