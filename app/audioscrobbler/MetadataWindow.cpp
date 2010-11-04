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
#include "MetadataWindow.h"

#include "Application.h"
#include "ScrobbleInfoFetcher.h"
#include "MediaDevices/DeviceScrobbler.h"
#include "../Widgets/ProfileWidget.h"
#include "../Widgets/ScrobbleControls.h"
#include "../Widgets/ScrobbleInfoWidget.h"
#include "../Widgets/TrackWidget.h"
#include "../Widgets/RecentTracksWidget.h"
#include "../Widgets/StatusBar.h"
#include "../Widgets/TitleBar.h"

#include "lib/unicorn/widgets/DataBox.h"
#include "lib/unicorn/widgets/MessageBar.h"
#include "lib/unicorn/widgets/GhostWidget.h"
#include "lib/unicorn/widgets/UserToolButton.h"
#include "lib/unicorn/StylableWidget.h"
#include "lib/unicorn/qtwin.h"
#include "lib/unicorn/layouts/SlideOverLayout.h"
#include "lib/listener/PlayerConnection.h"

#include <QLabel>
#include <QPushButton>
#include <QStatusBar>
#include <QSizeGrip>
#include <QTimer>
#include <QMenuBar>
#include <QSplitter>
#include <QScrollArea>
#include <QScrollBar>

MetadataWindow::MetadataWindow()
{
    setWindowTitle( "Audioscrobbler" );
    setAttribute( Qt::WA_TranslucentBackground );
    
    setWindowFlags( Qt::CustomizeWindowHint | Qt::FramelessWindowHint );
    
    setCentralWidget(new QWidget);

    QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
    layout->setSpacing( 0 );
    layout->setContentsMargins( 0, 0, 0, 0 );

    TitleBar* titleBar;
    layout->addWidget( titleBar = new TitleBar( this ));
    titleBar->setObjectName( "titleBar" );
    connect( titleBar, SIGNAL( closeClicked()), SLOT( close()));

    layout->addWidget( ui.splitter = new QSplitter( Qt::Vertical, this ), 1 );

    ui.nowPlaying = new TrackWidget();
    ui.nowPlaying->setObjectName("nowPlaying");

    ui.recentTracks = new RecentTracksWidget( lastfm::ws::Username, this );
    ui.recentTracks->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::MinimumExpanding );

    ui.tracks = new QWidget;
    QHBoxLayout* hl = new QHBoxLayout( ui.tracks );
    QVBoxLayout* vl = new QVBoxLayout;
    hl->setContentsMargins( 0, 0, 0, 0 );
    hl->setSpacing( 0 );
    vl->setContentsMargins( 0, 0, 0, 0 );
    vl->setSpacing( 0 );

    hl->addLayout( vl );

    QScrollBar* scrollBar = ui.recentTracks->scrollBar();
    hl->addWidget( scrollBar );

    vl->addWidget( ui.nowPlaying );
    vl->addWidget( ui.recentTracks );

    ui.splitter->addWidget( ui.tracks );

    ui.splitter->addWidget( ui.scrobbleInfo = new QWidget(this) );
    ui.scrobbleInfo->setObjectName( "NowScrobbling" );
    QVBoxLayout* nsLayout = new QVBoxLayout( ui.scrobbleInfo );
    nsLayout->setSpacing( 0 );
    nsLayout->setContentsMargins( 0, 0, 0, 0 );

    ui.splitter->setCollapsible( 0, false );
    setMinimumWidth( 410 );

    StylableWidget* statusBar;
    layout->addWidget( statusBar = new StatusBar( this ) );
    statusBar->setObjectName( "StatusBar" );



    addDragHandleWidget( titleBar );
    addDragHandleWidget( statusBar );

    setWindowTitle(tr("Last.fm Audioscrobbler"));
    setUnifiedTitleAndToolBarOnMac( true );
    setMinimumHeight( 80 );
    resize(20, 500);

    ui.message_bar = new MessageBar( centralWidget());

    finishUi();

    connect( qApp, SIGNAL( sessionChanged( unicorn::Session* ) ), SLOT( onSessionChanged( unicorn::Session* ) ) );
    connect( qApp, SIGNAL( trackStarted(Track, Track) ), SLOT( onTrackStarted(Track, Track) ) );
    connect( qApp, SIGNAL( paused() ), SLOT( onPaused() ) );
    connect( qApp, SIGNAL( resumed() ), SLOT( onResumed() ) );
    connect( qApp, SIGNAL( stopped() ), SLOT( onStopped() ) );

#ifdef NDEBUG
    menuBar()->hide();
#endif
}

void
MetadataWindow::onSessionChanged( unicorn::Session* session )
{
    ui.recentTracks->setUsername( session->userInfo().name() );

    // connect all the track widgets so we know when they are clicked
    for ( int i(0) ; i < ui.recentTracks->count() ; ++i )
        connect( ui.recentTracks->trackWidget(i), SIGNAL(clicked(TrackWidget*)), SLOT(onTrackClicked(TrackWidget*)));
}

void
MetadataWindow::onTrackStarted(const Track& t, const Track& previous)
{
    m_currentTrack = t;

    centralWidget()->setUpdatesEnabled(false);
    removeNowPlaying();
    ui.nowPlaying = new TrackWidget();
    ui.nowPlaying->setTrack( t );
    addNowPlaying( ui.nowPlaying );
    centralWidget()->setUpdatesEnabled(true);
}

void
MetadataWindow::onStopped()
{
    m_currentTrack = Track();

    centralWidget()->setUpdatesEnabled(false);
    removeNowPlaying();
    addNowPlaying( ui.nowPlaying = new TrackWidget() );
    centralWidget()->setUpdatesEnabled(true);
}

void
MetadataWindow::removeNowPlaying()
{
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(ui.tracks->layout()->itemAt(0)->layout());
    layout->removeWidget( ui.nowPlaying );

    disconnect( ui.nowPlaying->fetcher(), 0, this, 0 );
    disconnect( ui.nowPlaying->infoWidget(), 0, qApp, 0 );

    if ( ui.nowPlaying->track().scrobbleStatus() != lastfm::Track::Null )
        ui.recentTracks->addTrackWidget( ui.nowPlaying );
    else
        ui.nowPlaying->deleteLater();
}

void
MetadataWindow::addNowPlaying( TrackWidget* trackWidget )
{

    QVBoxLayout* layout = static_cast<QVBoxLayout*>(ui.tracks->layout()->itemAt(0)->layout());

    layout->insertWidget( 0, trackWidget );
    trackWidget->setObjectName("nowPlaying");

    connect( ui.nowPlaying->fetcher(), SIGNAL(trackGotInfo(XmlQuery)), SIGNAL(trackGotInfo(XmlQuery)));
    connect( ui.nowPlaying->fetcher(), SIGNAL(albumGotInfo(XmlQuery)), SIGNAL(albumGotInfo(XmlQuery)));
    connect( ui.nowPlaying->fetcher(), SIGNAL(artistGotInfo(XmlQuery)), SIGNAL(artistGotInfo(XmlQuery)));
    connect( ui.nowPlaying->fetcher(), SIGNAL(artistGotEvents(XmlQuery)), SIGNAL(artistGotEvents(XmlQuery)));
    connect( ui.nowPlaying->fetcher(), SIGNAL(trackGotTopFans(XmlQuery)), SIGNAL(trackGotTopFans(XmlQuery)));
    connect( ui.nowPlaying->fetcher(), SIGNAL(trackGotTags(XmlQuery)), SIGNAL(trackGotTags(XmlQuery)));
    connect( ui.nowPlaying->fetcher(), SIGNAL(finished()), SIGNAL(finished()));

    ui.nowPlaying->fetcher()->start();

    QVBoxLayout* nsLayout = static_cast<QVBoxLayout*>( ui.scrobbleInfo->layout() );
    if ( true )
    {
        // only swap the info widget if we were
        // viewing the now playing track
        while ( nsLayout->count() )
            nsLayout->takeAt( 0 )->widget()->hide();

        nsLayout->addWidget( ui.nowPlaying->infoWidget() );
        ui.nowPlaying->infoWidget()->show();
        connect( ui.nowPlaying->infoWidget(), SIGNAL(lovedStateChanged(bool)), qApp, SLOT(changeLovedState(bool)));
    }

    connect( trackWidget, SIGNAL(clicked(TrackWidget*)), SLOT(onTrackClicked(TrackWidget*)));
}

void
MetadataWindow::onResumed()
{
}


void
MetadataWindow::onPaused()
{
}

void
MetadataWindow::onTrackClicked( TrackWidget* clickedTrack )
{
    QVBoxLayout* nsLayout = static_cast<QVBoxLayout*>( ui.scrobbleInfo->layout() );

    while ( nsLayout->count() )
        nsLayout->takeAt( 0 )->widget()->hide();

    nsLayout->addWidget( clickedTrack->infoWidget() );
    clickedTrack->fetcher()->start();
    clickedTrack->infoWidget()->show();
}


void
MetadataWindow::addWinThumbBarButton( QAction* thumbButtonAction )
{
    m_buttons.append( thumbButtonAction );
}

void
MetadataWindow::addWinThumbBarButtons( QList<QAction*>& thumbButtonActions )
{
    foreach ( QAction* button, m_buttons )
        thumbButtonActions.append( button );
}

