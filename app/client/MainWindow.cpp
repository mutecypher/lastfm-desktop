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
#include "MainWindow.h"

#include "Application.h"
#include "Services/ScrobbleService.h"
#include "ScrobbleInfoFetcher.h"
#include "MediaDevices/DeviceScrobbler.h"
#include "../Widgets/ScrobbleControls.h"
#include "../Widgets/ScrobbleInfoWidget.h"
#include "../Widgets/NowPlayingItem.h"
#include "../Widgets/ActivityListWidget.h"
#include "../Widgets/TrackItem.h"
#include "../Widgets/SideBar.h"
#include "../Widgets/StatusBar.h"
#include "../Widgets/RadioListWidget.h"
#include "../Widgets/TitleBar.h"
#include "../Widgets/PlaybackControlsWidget.h"

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
#include <QStackedWidget>
#include <QStatusBar>
#include <QSizeGrip>
#include <QTimer>
#include <QMenuBar>
#include <QShortcut>
#include <QToolBar>
#include <QDockWidget>

MainWindow::MainWindow()
{
#ifdef Q_OS_MAC
    setUnifiedTitleAndToolBarOnMac( true );
#endif
    
    setCentralWidget(new QWidget);

    QHBoxLayout* layout = new QHBoxLayout( centralWidget() );
    layout->setSpacing( 0 );
    layout->setContentsMargins( 0, 0, 0, 0 );

    layout->addWidget( ui.sideBar = new SideBar( this ) );

    layout->addWidget( ui.stackedWidget = new QStackedWidget( this ) );

    connect( ui.sideBar, SIGNAL(currentChanged(int)), ui.stackedWidget, SLOT(setCurrentIndex(int)));

    //ui.playbackControls = new PlaybackControlsWidget( this );
    //ui.playbackControls->setObjectName("playbackControls");
    //ui.playbackControls->hide();

    //ui.nowPlaying = new NowPlayingItem( Track() );
    //ui.nowPlaying->setObjectName("nowPlaying");

    ui.stackedWidget->addWidget( ui.scrobbleInfo = new QWidget(this) );
    ui.scrobbleInfo->setObjectName( "NowScrobbling" );

    ui.stackedWidget->addWidget( ui.recentTracks = new ActivityListWidget( lastfm::ws::Username, this ) );
    ui.recentTracks->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::MinimumExpanding );

    ui.statusBar = new StatusBar( this );
    ui.statusBar->setObjectName( "StatusBar" );

#ifndef Q_OS_MAC
    ui.statusBar->setSizeGripEnabled( false );
#endif // Q_OS_MAC

    setWindowTitle( aApp->applicationName() );
    setUnifiedTitleAndToolBarOnMac( true );

    connect( qApp, SIGNAL( sessionChanged( unicorn::Session* ) ), SLOT( onSessionChanged( unicorn::Session* ) ) );
    connect( &ScrobbleService::instance(), SIGNAL( trackStarted(Track, Track) ), SLOT( onTrackStarted(Track, Track) ) );
    connect( &ScrobbleService::instance(), SIGNAL( paused() ), SLOT( onPaused() ) );
    connect( &ScrobbleService::instance(), SIGNAL( resumed() ), SLOT( onResumed() ) );
    connect( &ScrobbleService::instance(), SIGNAL( stopped() ), SLOT( onStopped() ) );

//    connect( ui.nowPlaying->fetcher(), SIGNAL(trackGotInfo(XmlQuery)), SIGNAL(trackGotInfo(XmlQuery)));
//    connect( ui.nowPlaying->fetcher(), SIGNAL(albumGotInfo(XmlQuery)), SIGNAL(albumGotInfo(XmlQuery)));
//    connect( ui.nowPlaying->fetcher(), SIGNAL(artistGotInfo(XmlQuery)), SIGNAL(artistGotInfo(XmlQuery)));
//    connect( ui.nowPlaying->fetcher(), SIGNAL(artistGotEvents(XmlQuery)), SIGNAL(artistGotEvents(XmlQuery)));
//    connect( ui.nowPlaying->fetcher(), SIGNAL(trackGotTopFans(XmlQuery)), SIGNAL(trackGotTopFans(XmlQuery)));
//    connect( ui.nowPlaying->fetcher(), SIGNAL(trackGotTags(XmlQuery)), SIGNAL(trackGotTags(XmlQuery)));
//    connect( ui.nowPlaying->fetcher(), SIGNAL(finished()), SIGNAL(finished()));

//    connect( ui.nowPlaying, SIGNAL(clicked(ActivityListItem*)), ui.recentTracks, SLOT(clearItemClicked()));

//    connect( ui.recentTracks, SIGNAL(itemClicked(ActivityListItem*)), SLOT(onItemClicked(ActivityListItem*)));
//    connect( ui.nowPlaying, SIGNAL(clicked(ActivityListItem*)), SLOT(onItemClicked(ActivityListItem*)));

    menuBar()->hide();

    //for some reason some of the stylesheet is not being applied properly unless reloaded
    //here. StyleSheets see very flaky to me. :s
    aApp->refreshStyleSheet();

    setMinimumWidth( 455 );

    setStatusBar( ui.statusBar );

    finishUi();
}

void
MainWindow::onSessionChanged( unicorn::Session* session )
{
    ui.recentTracks->setUsername( session->userInfo().name() );
}


void
MainWindow::onTrackStarted( const Track& t, const Track& /*previous*/ )
{
    // hide the playback controls if a radio track has started
//    if ( t.source() != Track::LastFmRadio )
//        ui.playbackControls->hide();

    newTrack( t );

    setWindowTitle( QApplication::applicationName() + " - " + t.toString() );
}


void
MainWindow::onStopped()
{
    newTrack( Track() );

    setWindowTitle( QApplication::applicationName() );
}

void
MainWindow::newTrack( const Track& track )
{
    addNowPlayingToActivityList();

    m_currentTrack = track;
//    ui.nowPlaying->setTrack( m_currentTrack );

    // only switch the info widget if we are
    // currently on the now playing track
//    if ( m_currentActivity == ui.nowPlaying )
//        onItemClicked( ui.nowPlaying );
}


void
MainWindow::addNowPlayingToActivityList()
{
//    if ( ui.nowPlaying->track() != Track()
//        && ui.nowPlaying->track().scrobbleStatus() != lastfm::Track::Null )
//    {
//        TrackItem* item = new TrackItem( *ui.nowPlaying );
//        ui.recentTracks->insertItem( item );
//    }
}


void
MainWindow::onResumed()
{
    setWindowTitle( QApplication::applicationName() + " - " + m_currentTrack.toString() );
}


void
MainWindow::onPaused()
{
    setWindowTitle( QApplication::applicationName() + " - Paused" );
}


void
MainWindow::onItemClicked( ActivityListItem* clickedItem )
{
    QVBoxLayout* infoLayout = static_cast<QVBoxLayout*>( ui.scrobbleInfo->layout() );

    while ( infoLayout->count() )
        infoLayout->takeAt( 0 )->widget()->hide();

    QWidget* widget = clickedItem->infoWidget();
    infoLayout->addWidget( widget );
    widget->show();

    m_currentActivity = clickedItem;
}


void
MainWindow::addWinThumbBarButton( QAction* thumbButtonAction )
{
    m_buttons.append( thumbButtonAction );
}


void
MainWindow::addWinThumbBarButtons( QList<QAction*>& thumbButtonActions )
{
    foreach ( QAction* button, m_buttons )
        thumbButtonActions.append( button );
}

