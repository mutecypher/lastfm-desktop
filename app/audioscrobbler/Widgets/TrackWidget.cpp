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
#include "TrackWidget.h"

#include <lastfm/ws.h>

TrackWidget::TrackWidget()
    :StylableWidget()
{
    setupUI();
}

TrackWidget::TrackWidget( const Track& track )
    :StylableWidget()
{
    setupUI();

    doSetTrack( track );

    connectTrack();

    updateTimestamp();
}

void
TrackWidget::setupUI()
{
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );

    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    ui.as = new QLabel();
    ui.as->setObjectName( "as" );

    movie.scrobbler_as = new Movie( ":/scrobbler_as.mng" );
    movie.scrobbler_paused = new Movie( ":/scrobbler_as_paused.mng" );
    movie.scrobbler_as->setCacheMode( QMovie::CacheAll );
    movie.scrobbler_paused->setCacheMode( QMovie::CacheAll );
    ui.as->setMovie( movie.scrobbler_as );

    layout->addWidget( ui.as );

    ui.trackTextArea = new QWidget( this );
    QHBoxLayout* h1 = new QHBoxLayout( ui.trackTextArea );
    h1->setSpacing( 0 );
    h1->setContentsMargins( 0, 0, 0, 0);

    h1->addWidget( ui.trackText = new QLabel( m_track.toString() ) );
    ui.trackText->setObjectName( "trackText" );

    h1->addWidget( ui.correction = new QLabel() );
    ui.correction->setObjectName( "correction" );
    ui.correction->hide();

    h1->addStretch( 1 );

    layout->addWidget( ui.trackTextArea, 1 );

    layout->addWidget( ui.love = new QLabel("love") );
    ui.love->setObjectName( "love" );
    ui.love->hide();

    layout->addWidget( ui.cog = new QToolButton() );
    ui.cog->setObjectName( "cog" );
    layout->addWidget( ui.ghostCog = new GhostWidget( ui.cog ) );
    ui.cog->hide();
    ui.ghostCog->show();

    QMenu* cogMenu = new QMenu( this );
    m_loveAction = cogMenu->addAction( QIcon(":/love-rest.png"), "Love", this, SLOT(onLoveClicked()) );
    m_loveAction->setCheckable( true );

    connect( cogMenu, SIGNAL(aboutToShow()), SIGNAL(cogMenuAboutToShow()));
    connect( cogMenu, SIGNAL(aboutToHide()), SIGNAL(cogMenuAboutToHide()));

    cogMenu->addAction( QIcon(":/tag-rest.png"), "Tag", this, SLOT(onTagClicked()) );
    cogMenu->addAction( QIcon(":/share-rest.png"), "Share", this, SLOT(onShareClicked()) );

    ui.cog->setMenu( cogMenu );

    layout->addWidget( ui.timestamp = new QLabel() );
    ui.timestamp->setObjectName( "timestamp" );

    m_timestampTimer = new QTimer( this );
    connect( m_timestampTimer, SIGNAL(timeout()), SLOT(updateTimestamp()));
}

void
TrackWidget::setFromIPodScrobble( const QList<Track>& tracks )
{
    QString format = (tracks.count() == 1 ?
        tr("%1 track from the iPod '%2'"):
        tr("%1 tracks from the iPod '%2'"));

    setText( format.arg( QString::number(tracks.count()), tracks[0].extra("deviceId") ) );
}

void
TrackWidget::setText( const QString& text )
{
    ui.trackText->setText( text );
}

QString
TrackWidget::text() const
{
    return ui.trackText->text();
}

void
TrackWidget::mousePressEvent( QMouseEvent * event )
{
    emit clicked(this);
}

ScrobbleInfoFetcher*
TrackWidget::fetcher() const
{
    return m_fetcher;
}

ScrobbleInfoWidget*
TrackWidget::infoWidget() const
{
    return m_infoWidget;
}

void
TrackWidget::setStatusToCurrentTrack()
{
    ui.timestamp->setText( aApp->currentConnection()->name() );

    ui.as->setMovie( movie.scrobbler_as );
    ui.as->movie()->start();

    if( m_stopWatch ) {
        disconnect( m_stopWatch, 0, this, 0 );
    }

    m_stopWatch = aApp->stopWatch();

    connect( m_stopWatch, SIGNAL(paused(bool)), SLOT( onWatchPaused(bool)) );
    connect( m_stopWatch, SIGNAL(timeout()), SLOT( onWatchFinished()));
    connect( m_stopWatch, SIGNAL(frameChanged(int)), SLOT(update()));
}

void
TrackWidget::onCorrected( QString correction )
{
    if ( m_track.corrected() )
    {


        ui.trackText->setText( correction );
        ui.correction->show();
        ui.correction->setToolTip( tr("Auto-corrected from: ") + m_track.toString( lastfm::Track::Original ) );
    }
    else
    {
        ui.trackText->setText( m_track.toString() );
        ui.correction->hide();
    }
}

void
TrackWidget::setTrack( const Track& track )
{
    doSetTrack( track );
    setStatusToCurrentTrack();
    connectTrack();
}

void
TrackWidget::doSetTrack( const Track& track )
{
    m_track = track;

    m_fetcher = new ScrobbleInfoFetcher( track, this );
    m_infoWidget = new ScrobbleInfoWidget( m_fetcher, this );
    m_infoWidget->hide();

    ui.love->setVisible( m_track.isLoved() );
    m_loveAction->setChecked( m_track.isLoved() );

    onCorrected( m_track.toString() );
}

void
TrackWidget::connectTrack()
{
    connect( m_track.signalProxy(), SIGNAL(corrected(QString)), SLOT(onCorrected(QString)));
    connect( m_track.signalProxy(), SIGNAL(loveToggled(bool)), SLOT(onLoveToggled(bool)));
    connect( m_track.signalProxy(), SIGNAL(loveToggled(bool)), m_loveAction, SLOT(setChecked(bool)));
    connect( m_track.signalProxy(), SIGNAL(scrobbleStatusChanged()), SLOT(onScrobbleStatusChanged()) );
}

void
TrackWidget::onWatchPaused( bool isPaused )
{
    if ( isPaused )
    {
        ui.timestamp->setText( tr( "%1 paused" ).arg( aApp->currentConnection()->name() ) );
        ui.as->setMovie( movie.scrobbler_paused );
        ui.as->movie()->start();
    }
    else
    {
        ui.timestamp->setText( aApp->currentConnection()->name() );
        ui.as->setMovie( movie.scrobbler_as );
        ui.as->movie()->start();
    }

    update();
}

void
TrackWidget::onWatchFinished()
{
    connect( ui.as->movie(), SIGNAL(loopFinished()), ui.as->movie(), SLOT(stop()));
    ui.timestamp->setText( tr( "Track Scrobbled" ));
}

void
TrackWidget::paintEvent( QPaintEvent* event )
{
    StylableWidget::paintEvent( event );

    if ( m_track.sameObject( aApp->currentTrack() ) )
    {
        // This is the current track so draw the progress
        m_stopWatch = aApp->stopWatch();

        QPainter p( this );
        p.setPen( QColor( Qt::transparent ));
        p.setBrush( QColor( 0, 0, 0, 60 ));

        float percentage = (m_stopWatch->elapsed()/1000.0f) / m_stopWatch->scrobblePoint();
        p.drawRect( 0, 0, width() * percentage , height());
    }
}

void
TrackWidget::enterEvent( class QEvent* )
{
    setUpdatesEnabled(false);
    ui.ghostCog->hide();
    ui.cog->show();
    setUpdatesEnabled(true);
}

void
TrackWidget::leaveEvent( class QEvent* )
{
    setUpdatesEnabled(false);
    ui.cog->hide();
    ui.ghostCog->show();
    setUpdatesEnabled(true);
}

void
TrackWidget::resizeEvent(QResizeEvent* )
{
    int width =  ui.trackTextArea->width();
    if (ui.correction->isVisible() ) width -=  ui.correction->width();


    QFontMetrics fm( ui.trackText->font() );
    ui.trackText->setText( fm.elidedText ( m_track.toString(), Qt::ElideRight, width) );
}

void
TrackWidget::updateTimestamp()
{
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
TrackWidget::onLoveClicked()
{
    MutableTrack track = MutableTrack( m_track );

    if ( track.isLoved() )
        track.unlove();
    else
        track.love();
}

void
TrackWidget::onTagClicked()
{
    TagDialog* tagDialog = new TagDialog( m_track, this );
    tagDialog->show();
}


void
TrackWidget::onShareClicked()
{
    ShareDialog* shareDialog = new ShareDialog( m_track, this );
    shareDialog->show();
}


void
TrackWidget::onLoveToggled( bool loved )
{
    ui.love->setVisible( loved );
}

void
TrackWidget::onScrobbleStatusChanged()
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

    ui.trackText->setStyle(QApplication::style());
}

