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
    doSetTrack( that.m_track );
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

    /// Set some track specific data for the UI
    ui.love->setVisible( track.isLoved() );

    onCorrected( track.toString() );

    /// conenct up the new track
    connect( track.signalProxy(), SIGNAL(corrected(QString)), SLOT(onCorrected(QString)));
    connect( track.signalProxy(), SIGNAL(loveToggled(bool)), SLOT(onLoveToggled(bool)));
    connect( track.signalProxy(), SIGNAL(scrobbleStatusChanged()), SLOT(onScrobbleStatusChanged()) );

    // make sure we tell people when we change
    connect( track.signalProxy(), SIGNAL(loveToggled(bool)), SIGNAL(changed()));
    connect( track.signalProxy(), SIGNAL(scrobbleStatusChanged()), SIGNAL(changed()));
    connect( track.signalProxy(), SIGNAL(corrected(QString)), SIGNAL(changed()));

    // track items need their timestamp updated as they grow old
    m_timestamp = track.timestamp();

    updateTimestamp();
}


QDomElement
TrackItem::toDomElement( QDomDocument xml ) const
{
    return m_track.toDomElement( xml );
}


QWidget*
TrackItem::infoWidget() const
{
    // Start the fetcher a bit later so that
    // we switch to the item immediately
    QTimer::singleShot( 10, m_fetcher, SLOT(start()));

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
TrackItem::updateTimestamp()
{
    if (!m_timestampTimer)
    {
        m_timestampTimer = new QTimer( this );
        connect( m_timestampTimer, SIGNAL(timeout()), SLOT(updateTimestamp()));
    }

    ui.as->clear();

    QDateTime now = QDateTime::currentDateTime();

    // Full time in the tool tip
    ui.timestamp->setToolTip(m_track.timestamp().toString( "ddd h:ssap" ));

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

