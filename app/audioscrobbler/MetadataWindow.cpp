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
#include "../Widgets/NowPlayingItem.h"
#include "../Widgets/ActivityListWidget.h"
#include "../Widgets/TrackItem.h"
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

    ui.nowPlaying = new NowPlayingItem( Track() );
    ui.nowPlaying->setObjectName("nowPlaying");

    ui.recentTracks = new ActivityListWidget( lastfm::ws::Username, this );
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
    ui.splitter->handle( 1 )->setAutoFillBackground( true );
    setMinimumWidth( 410 );

    StylableWidget* statusBar;
    layout->addWidget( statusBar = new StatusBar( this ) );
    statusBar->setObjectName( "StatusBar" );

    addDragHandleWidget( titleBar );
    addDragHandleWidget( statusBar );

    setWindowTitle(tr("The Scrobbler"));
    setUnifiedTitleAndToolBarOnMac( true );
    setMinimumHeight( 80 );
    resize(20, 500);

    ui.message_bar = new MessageBar( centralWidget());

    onItemClicked( ui.nowPlaying );

    finishUi();

    connect( qApp, SIGNAL( sessionChanged( unicorn::Session* ) ), SLOT( onSessionChanged( unicorn::Session* ) ) );
    connect( qApp, SIGNAL( trackStarted(Track, Track) ), SLOT( onTrackStarted(Track, Track) ) );
    connect( qApp, SIGNAL( paused() ), SLOT( onPaused() ) );
    connect( qApp, SIGNAL( resumed() ), SLOT( onResumed() ) );
    connect( qApp, SIGNAL( stopped() ), SLOT( onStopped() ) );

    connect( ui.nowPlaying->fetcher(), SIGNAL(trackGotInfo(XmlQuery)), SIGNAL(trackGotInfo(XmlQuery)));
    connect( ui.nowPlaying->fetcher(), SIGNAL(albumGotInfo(XmlQuery)), SIGNAL(albumGotInfo(XmlQuery)));
    connect( ui.nowPlaying->fetcher(), SIGNAL(artistGotInfo(XmlQuery)), SIGNAL(artistGotInfo(XmlQuery)));
    connect( ui.nowPlaying->fetcher(), SIGNAL(artistGotEvents(XmlQuery)), SIGNAL(artistGotEvents(XmlQuery)));
    connect( ui.nowPlaying->fetcher(), SIGNAL(trackGotTopFans(XmlQuery)), SIGNAL(trackGotTopFans(XmlQuery)));
    connect( ui.nowPlaying->fetcher(), SIGNAL(trackGotTags(XmlQuery)), SIGNAL(trackGotTags(XmlQuery)));
    connect( ui.nowPlaying->fetcher(), SIGNAL(finished()), SIGNAL(finished()));

    connect( ui.nowPlaying, SIGNAL(clicked(ActivityListItem*)), ui.recentTracks, SLOT(clearItemClicked()));

    connect( ui.recentTracks, SIGNAL(itemClicked(ActivityListItem*)), SLOT(onItemClicked(ActivityListItem*)));
    connect( ui.nowPlaying, SIGNAL(clicked(ActivityListItem*)), SLOT(onItemClicked(ActivityListItem*)));

#ifdef NDEBUG
    menuBar()->hide();
#endif
}


void
MetadataWindow::onSessionChanged( unicorn::Session* session )
{
    ui.recentTracks->setUsername( session->userInfo().name() );
}


void
MetadataWindow::onTrackStarted( const Track& t, const Track& /*previous*/ )
{
    newTrack( t );
}


void
MetadataWindow::onStopped()
{
    newTrack( Track() );
}

void
MetadataWindow::newTrack( const Track& track )
{
    addNowPlayingToActivityList();

    m_currentTrack = track;
    ui.nowPlaying->setTrack( m_currentTrack );

    // only switch the info widget if we are
    // currently on the now playing track
    if ( m_currentActivity == ui.nowPlaying )
        onItemClicked( ui.nowPlaying );
}


void
MetadataWindow::addNowPlayingToActivityList()
{
    if ( ui.nowPlaying->track() != Track()
        && ui.nowPlaying->track().scrobbleStatus() != lastfm::Track::Null )
    {
        TrackItem* item = new TrackItem( *ui.nowPlaying );
        ui.recentTracks->addItem( item );
    }
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
MetadataWindow::onItemClicked( ActivityListItem* clickedItem )
{
    QVBoxLayout* infoLayout = static_cast<QVBoxLayout*>( ui.scrobbleInfo->layout() );

    while ( infoLayout->count() )
        infoLayout->takeAt( 0 )->widget()->hide();

    QWidget* widget = clickedItem->infoWidget();
    widget->show();
    infoLayout->addWidget( widget );

    m_currentActivity = clickedItem;
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

