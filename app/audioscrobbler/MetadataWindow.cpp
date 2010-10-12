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
#include "MediaDevices/DeviceScrobbler.h"
#include "../Widgets/ProfileWidget.h"
#include "../Widgets/ScrobbleControls.h"
#include "../Widgets/ScrobbleInfoWidget.h"
#include "../Widgets/TrackWidget.h"
#include "../Widgets/RecentTracksWidget.h"

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


TitleBar::TitleBar( const QString& /*title*/ )
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    QPushButton* pb = new QPushButton( "Close" );
    pb->setShortcut( Qt::CTRL + Qt::Key_H );
#ifdef Q_OS_MAC
    layout->addWidget( pb );
#endif
    connect( pb, SIGNAL(clicked()), SIGNAL( closeClicked()));
    pb->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
    pb->setFlat( true );
    layout->addWidget( m_inetStatus = new QLabel( "Online", this ) );
    layout->addWidget( m_iPodStatus = new QLabel( "", this ));
    m_inetStatus->setStyleSheet( "color: #cccccc" );
    m_iPodStatus->setStyleSheet( "color: #cccccc" );
    connect( qApp, SIGNAL( internetConnectionDown() ), this, SLOT( onConnectionDown() ) );
    connect( qApp, SIGNAL( internetConnectionUp() ), this, SLOT( onConnectionUp() ) );
    DeviceScrobbler* deviceScrobbler = qobject_cast<audioscrobbler::Application*>(qApp)->deviceScrobbler();
    connect(deviceScrobbler, SIGNAL( processingScrobbles()), SLOT( onProcessingScrobbles()));
    connect(deviceScrobbler, SIGNAL( detectedIPod( QString )), SLOT( onIPodDetected()));
    connect(deviceScrobbler, SIGNAL( foundScrobbles( QList<Track> )), m_iPodStatus, SLOT( clear()));
    connect(deviceScrobbler, SIGNAL( noScrobblesFound()), m_iPodStatus, SLOT( clear()));
    //QLabel* l;
    //layout->addWidget( l = new QLabel( title, this ));
    //l->setAlignment( Qt::AlignCenter );
    layout->addStretch( 1 );

    m_inetStatus->setAlignment( Qt::AlignLeft );
#ifndef Q_OS_MAC
    layout->addWidget( pb );
#endif
}

void
TitleBar::onConnectionUp()
{
    m_inetStatus->setText( "Online" );
}

void
TitleBar::onConnectionDown()
{
    m_inetStatus->setText( "Offline" );
}

void
TitleBar::onIPodDetected( )
{
    m_iPodStatus->setText( "iPod Detected.." );
}

void
TitleBar::onProcessingScrobbles()
{
    m_iPodStatus->setText( "Processing iPod scrobbles..." );
}

void 
TitleBar::onScrobblesFound()
{
    m_iPodStatus->setText( "Found iPod Scrobbles" );

}


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
    qobject_cast<QBoxLayout*>(centralWidget()->layout())->addWidget( titleBar = new TitleBar("Audioscrobbler"), 0, Qt::AlignBottom );
    connect( titleBar, SIGNAL( closeClicked()), SLOT( close()));

    ui.nowPlaying = new TrackWidget();
    ui.nowPlaying->setObjectName("nowPlaying");
    layout->addWidget( ui.nowPlaying );

    layout->addWidget( ui.splitter = new QSplitter( Qt::Vertical, this ), 1 );


    //HACK: on KDE, the tab bar seems to inherit the app background color resulting in
    //a dark font color over dark background.
    //if ( ui.tabs->tabBar()->palette().color( ui.tabs->tabBar()->backgroundRole() ).name() ==  "#e0dfde" )
    //{
    //    ui.tabs->tabBar()->setStyleSheet( "color: #cccccc" );
    //}

    ui.recentTracks = new RecentTracksWidget( lastfm::ws::Username, this );
    ui.recentTracks->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::MinimumExpanding );

    ui.splitter->addWidget( ui.recentTracks );

    ui.nowScrobbling = new ScrobbleInfoWidget( aApp->fetcher(), centralWidget() );
    ui.nowScrobbling->setObjectName( "NowScrobbling" );

    ui.splitter->addWidget( ui.nowScrobbling );

    connect( ui.nowScrobbling, SIGNAL(lovedStateChanged(bool)), qApp, SLOT(changeLovedState(bool)));

    setMinimumWidth( 410 );

    StylableWidget* statusBar = new StylableWidget( this );
    statusBar->setObjectName( "StatusBar" );
    {
        statusBar->setContentsMargins( 0, 0, 0, 0 );

        QHBoxLayout* sb = new QHBoxLayout( statusBar );
        QSizeGrip* sg = new QSizeGrip( this );
        sb->addWidget( sg, 0 , Qt::AlignBottom | Qt::AlignRight );
        layout->addWidget( statusBar );
    }

    addDragHandleWidget( titleBar );
    addDragHandleWidget( ui.nowPlaying );
    //addDragHandleWidget( ui.profile );
    addDragHandleWidget( ui.nowScrobbling );
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
}

void
MetadataWindow::onTrackStarted(const Track& t, const Track& previous)
{
    m_currentTrack = t;
}


void
MetadataWindow::onStopped()
{
    m_currentTrack = Track();
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

