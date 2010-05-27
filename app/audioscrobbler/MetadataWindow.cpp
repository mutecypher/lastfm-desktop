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

#include "ScrobbleStatus.h"
#include "ScrobbleControls.h"
#include "Application.h"
#include "ProfileWidget.h"
#include "ScrobbleInfoWidget.h"

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


TitleBar::TitleBar( const QString& title )
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    QPushButton* pb = new QPushButton( "Close" );
#ifdef Q_OS_MAC
    pb->setShortcut( Qt::CTRL + Qt::Key_H );
    layout->addWidget( pb );
#endif
    connect( pb, SIGNAL(clicked()), SIGNAL( closeClicked()));
    pb->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
    pb->setFlat( true );
    QLabel* l;
    layout->addWidget( l = new QLabel( title ));
    l->setAlignment( Qt::AlignCenter );
#ifndef Q_OS_MAC
    pb->setShortcut( Qt::CTRL + Qt::Key_H );
    layout->addWidget( pb );
#endif
}

MetadataWindow::MetadataWindow()
               :layout( new SlideOverLayout())
{
    setWindowTitle( "Audioscrobbler" );
    setAttribute( Qt::WA_TranslucentBackground );
    
    setWindowFlags( Qt::CustomizeWindowHint | Qt::FramelessWindowHint );
    
    setCentralWidget(new QWidget);

    new QVBoxLayout( centralWidget());
    centralWidget()->layout()->setSpacing( 0 );
    centralWidget()->layout()->setContentsMargins( 0, 0, 0, 0 );

    TitleBar* titleBar;
    qobject_cast<QBoxLayout*>(centralWidget()->layout())->addWidget( titleBar = new TitleBar("Audioscrobbler"), 0, Qt::AlignBottom );
    connect( titleBar, SIGNAL( closeClicked()), SLOT( close()));

    ui.now_playing_source = new ScrobbleStatus();
    ui.now_playing_source->setContentsMargins( 0, 0, 0, 0 );
    QWidget* hb = new StylableWidget();
    hb->setObjectName( "ScrobbleStatusRow" );
    new QHBoxLayout( hb );
    hb->layout()->setContentsMargins( 0, 0, 0, 0 );
    hb->layout()->setSpacing( 0 );

    hb->layout()->addWidget( ui.tabBar = new QTabBar());
    ui.tabBar->insertTab( TAB_PROFILE, tr( "Profile" ) );
    ui.tabBar->insertTab( TAB_INFO, tr( "Info" ) );
    connect( ui.tabBar, SIGNAL( currentChanged( int )), SLOT( onTabChanged( int )));
    hb->layout()->addWidget( ui.now_playing_source );

    centralWidget()->layout()->addWidget(hb);
    connect( ui.now_playing_source, SIGNAL( clicked()), SLOT( showNowScrobbling()));

    stack.nowScrobbling = new ScrobbleInfoWidget( centralWidget() );
    stack.nowScrobbling->setObjectName( "NowScrobbling" );
    layout->addWidget( stack.nowScrobbling );

    connect(stack.nowScrobbling, SIGNAL(lovedStateChanged(bool)), qApp, SLOT(changeLovedState(bool)));

    ui.now_playing_source->setObjectName("now_playing");
    ui.now_playing_source->setFixedHeight( 22 );
    QWidget* slideOverWidget = new StylableWidget();
    slideOverWidget->setObjectName( "slideOverContainer" );
    slideOverWidget->setLayout( layout );
    centralWidget()->layout()->addWidget( slideOverWidget );
    layout->addWidget( stack.profile = new ProfileWidget());

    setCurrentWidget( stack.profile );


    setMinimumWidth( 410 );

    StylableWidget* statusBar = new StylableWidget( this );
    statusBar->setObjectName( "StatusBar" );
    {
        statusBar->setContentsMargins( 0, 0, 0, 0 );

        /*ui.userButton = new UserToolButton();
        ui.userButton->setChecked( true );
        connect( ui.userButton, SIGNAL(toggled( bool )), SLOT(toggleProfile( bool )));*/
        QHBoxLayout* sb = new QHBoxLayout( statusBar );
        QSizeGrip* sg = new QSizeGrip( this );

        //sb->addWidget( ui.userButton );
        //FIXME: this code is duplicated in the radio too
        //In order to compensate for the sizer grip on the bottom right
        //of the window, an empty QWidget is added as a spacer.
        if( sg )
            sb->addWidget( new GhostWidget( sg ) );

        sb->addWidget( new QWidget(), 1 );
        sb->addWidget( ui.sc = new ScrobbleControls());
        sb->addWidget( new QWidget(), 1 );
        //sb->addWidget( new GhostWidget( ui.userButton ));
        sb->addWidget( sg, 0 , Qt::AlignBottom | Qt::AlignRight );
        centralWidget()->layout()->addWidget( statusBar );
    }

    addDragHandleWidget( titleBar );
#ifdef Q_OS_MAC
    addDragHandleWidget( ui.now_playing_source );
    //addDragHandleWidget( nav );
    addDragHandleWidget( stack.profile );
    addDragHandleWidget( stack.nowScrobbling );
    addDragHandleWidget( statusBar );
#endif

    setWindowTitle(tr("Last.fm Audioscrobbler"));
    setUnifiedTitleAndToolBarOnMac( true );
    setMinimumHeight( 80 );
    resize(20, 500);

    ui.message_bar = new MessageBar( centralWidget());

    finishUi();

    menuBar()->hide();
}


void
MetadataWindow::onTrackStarted(const Track& t, const Track& previous)
{
    //ui.userButton->setChecked( false );
    ui.tabBar->setCurrentIndex( TAB_INFO );
    toggleProfile( false );
    ui.now_playing_source->onTrackStarted( t, previous );
    m_currentTrack = t;
}


void
MetadataWindow::onStopped()
{
    setCurrentWidget( stack.profile );
    ui.now_playing_source->onTrackStopped();
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
MetadataWindow::setCurrentWidget( QWidget* w )
{
    centralWidget()->findChild<SlideOverLayout*>()->revealWidget( w );
}

void
MetadataWindow::toggleProfile( bool show )
{
    if( show )
        setCurrentWidget( stack.profile );
    else
        setCurrentWidget( stack.nowScrobbling );
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
    ui.tabBar->setCurrentIndex( TAB_INFO );
    toggleProfile( false );
}
