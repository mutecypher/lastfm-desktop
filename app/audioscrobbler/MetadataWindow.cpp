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
#include "../Widgets/ScrobbleStatus.h"
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
#include <QTabWidget>


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

    ui.now_playing_source = new ScrobbleStatus();
    ui.now_playing_source->setContentsMargins( 0, 0, 0, 0 );
    ui.now_playing_source->setObjectName("now_playing");
    ui.now_playing_source->setFixedHeight( 22 );

    QWidget* hb = new StylableWidget();
    hb->setObjectName( "ScrobbleStatusRow" );
    new QHBoxLayout( hb );
    hb->layout()->setContentsMargins( 0, 0, 0, 0 );
    hb->layout()->setSpacing( 0 );
    hb->layout()->addWidget( ui.now_playing_source );

    layout->addWidget(hb);
    layout->addWidget( ui.profile = new ProfileWidget() );
    layout->addWidget( ui.tabs = new QTabWidget( this ), 1 );

    //HACK: on KDE, the tab bar seems to inherit the app background color resulting in
    //a dark font color over dark background.
    //if ( ui.tabs->tabBar()->palette().color( ui.tabs->tabBar()->backgroundRole() ).name() ==  "#e0dfde" )
    //{
    //    ui.tabs->tabBar()->setStyleSheet( "color: #cccccc" );
    //}

    ui.recentTracks = new RecentTracksWidget( lastfm::ws::Username, this );
    ui.recentTracks->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::MinimumExpanding );

    DataBox* recentTrackBox = new DataBox( tr( "Recently scrobbled tracks" ), ui.recentTracks );
    recentTrackBox->setObjectName( "recentTracks" );

    ui.tabs->addTab( recentTrackBox, tr( "Recent tracks" ) );

    ui.nowScrobbling = new ScrobbleInfoWidget( centralWidget() );
    ui.nowScrobbling->setObjectName( "NowScrobbling" );

    ui.tabs->addTab( ui.nowScrobbling, tr( "Info" ) );

    ui.tabs->setTabEnabled( TAB_INFO, false );

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
    addDragHandleWidget( ui.now_playing_source );
    addDragHandleWidget( ui.profile );
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
    ui.tabs->setTabEnabled ( TAB_INFO, true );

    if ( m_currentTrack.isNull() )
    {
        // We are starting form a stopped state
        // so switch to the info view
        ui.tabs->setCurrentIndex( TAB_INFO );
        toggleProfile( false );
    }

    m_currentTrack = t;
}


void
MetadataWindow::onStopped()
{
    setCurrentWidget( ui.profile );
    ui.tabs->setCurrentIndex( TAB_PROFILE );
    m_currentTrack = Track();
    ui.tabs->setTabEnabled ( TAB_INFO, false );
}


void
MetadataWindow::onResumed()
{
    // Switch back to the info view when you resume. We never
    // actually stop with iTunes so this is more consistent.
    ui.tabs->setTabEnabled ( TAB_INFO, true );
    ui.tabs->setCurrentIndex( TAB_INFO );
    toggleProfile( false );
}


void
MetadataWindow::onPaused()
{
    setCurrentWidget( ui.profile );
    ui.tabs->setTabEnabled ( TAB_INFO, false );
}


void
MetadataWindow::setCurrentWidget( QWidget* w )
{
    int index = ui.tabs->indexOf( w );

    // Make sure we dont's switch to the same widget
    // because it makes ugly things happen
    if ( index != -1 && index != ui.tabs->currentIndex() )
        ui.tabs->setCurrentIndex( index );
}

void
MetadataWindow::toggleProfile( bool show )
{
    if( show )
        setCurrentWidget( ui.profile );
    else
        setCurrentWidget( ui.nowScrobbling );
}

void 
MetadataWindow::onTabChanged( int index )
{
    if( index == 0 ) {
        toggleProfile( true );
    } else {
        toggleProfile( false );
    }
}

void 
MetadataWindow::showNowScrobbling()
{
    //ui.userButton->setChecked( false );
    ui.tabs->setCurrentIndex( TAB_INFO );
    toggleProfile( false );
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

