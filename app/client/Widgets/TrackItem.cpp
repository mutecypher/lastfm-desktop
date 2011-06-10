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
#include "../ScrobbleInfoFetcher.h"
#include "../Services/RadioService/RadioService.h"

#include "ScrobbleInfoWidget.h"
#include "TrackItem.h"

#include <lastfm/ws.h>

#include "ui_ActivityListItem.h"

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

    disconnect( ui->love, 0, this, 0 );
    connect( ui->love, SIGNAL(toggled(bool)), this, SLOT(onLoveChanged( bool )));

    ui->love->setVisible( !track.isNull() );

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
    setLoveChecked( m_track.isLoved() );
    onCorrected( m_track.toString() );
    connectTrack();

    // track items need their timestamp updated as they grow old
    m_timestamp = m_track.timestamp();
    updateTimestamp();

    ui->player->setText( m_track.source() == Track::LastFmRadio ?
                         tr( "Last.fm Radio" ):
                         m_track.extra( "playerName" ) );
}

void
TrackItem::connectTrack()
{
    connect( m_track.signalProxy(), SIGNAL(corrected(QString)), SLOT(onCorrected(QString)));
    connect( m_track.signalProxy(), SIGNAL(loveToggled(bool)), SLOT(setLoveChecked(bool)));
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
        ui->correction->show();
        ui->correction->setToolTip( tr("Auto-corrected from: ") + m_track.toString( lastfm::Track::Original ) );
    }
    else
    {
        setText( m_track.toString() );
        ui->correction->hide();
    }
}


void
TrackItem::setLoveChecked( bool checked )
{
    /// This just changes the state of the love button

    ui->love->setChecked( checked );

    if ( checked )
        ui->love->setToolTip( tr( "Unlove track" ) );
    else
        ui->love->setToolTip( tr( "Love track" ) );
}

void
TrackItem::onLoveChanged( bool checked )
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
    case Track::None:
        // don't do anything
        break;
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
        // if it was none of the above errors (scrobble filters etc)
        // it was an error with the scrobble submission as a whole (malformed request etc)
        setToolTip( m_track.scrobbleErrorText() );
        break;
    }

    ui->text->setStyle(QApplication::style());
}

QString userLibrary( const QString& user, const QString& artist )
{
    return Label::anchor( QString("http://www.last.fm/user/%1/library/music/%2").arg( user, artist ), user );
}

QString
TrackItem::contextString( const Track& track )
{
    QString contextString;

    lastfm::TrackContext context = track.context();

    if ( context.values().count() > 0 )
    {
        switch ( context.type() )
        {
        case lastfm::TrackContext::Artist:
            {
            switch ( context.values().count() )
                {
                default:
                case 1: contextString = tr( "because you listen to %1" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ) ); break;
                case 2: contextString = tr( "because you listen to %1 and %2" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ), Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ) ); break;
                case 3: contextString = tr( "because you listen to %1, %2, and %3" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ) , Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ), Label::anchor( Artist( context.values().at(2) ).www().toString(), context.values().at(2) ) ); break;
                case 4: contextString = tr( "because you listen to %1, %2, %3, and %4" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ), Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ), Label::anchor( Artist( context.values().at(2) ).www().toString(), context.values().at(2) ), Label::anchor( Artist( context.values().at(3) ).www().toString(), context.values().at(3) ) ); break;
                case 5: contextString = tr( "because you listen to %1, %2, %3, %4, and %5" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ), Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ), Label::anchor( Artist( context.values().at(2) ).www().toString(), context.values().at(2) ), Label::anchor( Artist( context.values().at(3) ).www().toString(), context.values().at(3) ), Label::anchor( Artist( context.values().at(4) ).www().toString(), context.values().at(4) ) ); break;
                }
            }
            break;
        case lastfm::TrackContext::User:
            // Whitelist multi-user station
            if ( !RadioService::instance().station().url().startsWith("lastfm://users/") )
                break;
        case lastfm::TrackContext::Friend:
        case lastfm::TrackContext::Neighbour:
            {
            switch ( context.values().count() )
                {
                default:
                case 1: contextString = tr( "from %2%1s library" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ) ); break;
                case 2: contextString = tr( "from %2 and %3%1s libraries" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ), userLibrary( context.values().at(1), track.artist().name() ) ); break;
                case 3: contextString = tr( "from %2, %3, and %4%1s libraries" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ), userLibrary( context.values().at(1), track.artist().name() ), userLibrary( context.values().at(2), track.artist().name() ) ); break;
                case 4: contextString = tr( "from %2, %3, %4, and %5%1s libraries" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ),userLibrary(  context.values().at(1), track.artist().name() ), userLibrary( context.values().at(2), track.artist().name() ), userLibrary( context.values().at(3), track.artist().name() ) ); break;
                case 5: contextString = tr( "from %2, %3, %4, %5, and %6%1s libraries" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ), userLibrary( context.values().at(1), track.artist().name() ), userLibrary( context.values().at(2), track.artist().name() ), userLibrary( context.values().at(3), track.artist().name() ), userLibrary( context.values().at(4), track.artist().name() ) ); break;
                }
            }
            break;
        }
    }

    return contextString;
}

