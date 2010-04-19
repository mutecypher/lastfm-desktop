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
#include "lib/unicorn/StylableWidget.h"
#include "lib/unicorn/qtwin.h"
#include "lib/unicorn/layouts/SideBySideLayout.h"
#include "lib/listener/PlayerConnection.h"

#include <QLabel>
#include <QPushButton>
#include <QStatusBar>
#include <QSizeGrip>


TitleBar::TitleBar( const QString& title )
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    QPushButton* pb;
    layout->addWidget( pb = new QPushButton( "" ));
    connect( pb, SIGNAL(clicked()), SIGNAL( closeClicked()));
    pb->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
    pb->setFlat( true );
    QLabel* l;
    layout->addWidget( l = new QLabel( title ));
    l->setAlignment( Qt::AlignCenter );
}

MetadataWindow::MetadataWindow()
{
    setWindowTitle( "Audioscrobbler" );
    setAttribute( Qt::WA_TranslucentBackground );
    QPalette pal(palette());
    pal.setColor( QPalette::Window, QColor( 0, 0, 0, 0 ));
    setPalette( pal );
    setAutoFillBackground( true );
    
    setAttribute( Qt::WA_MacAlwaysShowToolWindow );
	
#ifdef Q_OS_MAC
	setWindowFlags( Qt::CustomizeWindowHint );
#else
	setWindowFlags( Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint );
#endif	

    //Enable aero blurry window effect:
    QtWin::extendFrameIntoClientArea( this );
    
    setCentralWidget(new QWidget);

    new QVBoxLayout( centralWidget());
    centralWidget()->layout()->setSpacing( 0 );
    centralWidget()->layout()->setContentsMargins( 0, 0, 0, 0 );

#ifdef Q_OS_MAC   
    TitleBar* titleBar;
    qobject_cast<QBoxLayout*>(centralWidget()->layout())->addWidget( titleBar = new TitleBar("Audioscrobbler"), 0, Qt::AlignBottom );
    connect( titleBar, SIGNAL( closeClicked()), SLOT( close()));
#endif

    SideBySideLayout* stackLayout = new SideBySideLayout();
    centralWidget()->layout()->addWidget(ui.now_playing_source = new ScrobbleStatus());
    QWidget* nav;
    centralWidget()->layout()->addWidget( nav = new StylableWidget());
    nav->setObjectName( "navigation" );
   
    ui.now_playing_source->setObjectName("now_playing");
    ui.now_playing_source->setFixedHeight( 22 );
    qobject_cast<QBoxLayout*>(centralWidget()->layout())->addLayout( stackLayout );
    stackLayout->addWidget( stack.profile = new ProfileWidget());

    {
        QHBoxLayout* nl = new QHBoxLayout( nav );
        nl->addWidget( ui.nav.profile = new QPushButton( tr( "Profile" )), 0, Qt::AlignLeft );
        nl->addWidget( ui.nav.nowScrobbling = new QPushButton( tr( "Now Scrobbling" )),0, Qt::AlignRight);
        connect( ui.nav.profile, SIGNAL( clicked()), SLOT( showProfile()));
        connect( ui.nav.nowScrobbling, SIGNAL( clicked()), SLOT( showNowScrobbling()));
    }
    

    stack.nowScrobbling = new ScrobbleInfoWidget( centralWidget() );
    stack.nowScrobbling->setObjectName( "NowScrobbling" );
    stackLayout->addWidget( stack.nowScrobbling );
    setCurrentWidget( stack.profile );

    setMinimumWidth( 410 );


    // status bar and scrobble controls
    QStatusBar* statusBar = new QStatusBar( this );
    {
        statusBar->setContentsMargins( 0, 0, 0, 0 );
#ifdef Q_WS_WIN
        statusBar->setSizeGripEnabled( false );
#else
        //FIXME: this code is duplicated in the radio too
        //In order to compensate for the sizer grip on the bottom right
        //of the window, an empty QWidget is added as a spacer.
        QSizeGrip* sg = statusBar->findChild<QSizeGrip *>();
        if( sg ) {
            int gripWidth = sg->sizeHint().width();
            QWidget* w = new QWidget( statusBar );
            w->setFixedWidth( gripWidth );
            statusBar->addWidget( w );
        }
#endif
        //Seemingly the only way to get a central widget in a QStatusBar
        //is to add an empty widget either side with a stretch value.
        statusBar->addWidget( new QWidget( statusBar), 1 );
        statusBar->addWidget( ui.sc = new ScrobbleControls());
        statusBar->addWidget( new QWidget( statusBar), 1 );
        setStatusBar( statusBar );
    }

#ifdef Q_OS_MAC
    addDragHandleWidget( ui.now_playing_source );
    addDragHandleWidget( titleBar );
    addDragHandleWidget( nav );
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
}


void
MetadataWindow::onTrackStarted(const Track& t, const Track& previous)
{
    setCurrentWidget( stack.nowScrobbling );
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
    if( w == stack.nowScrobbling ) {
        ui.nav.profile->setVisible( true );
        ui.nav.nowScrobbling->setVisible( false );
    } else if( w == stack.profile ) {
        ui.nav.profile->setVisible( false );
        PlayerConnection* connection = qobject_cast<audioscrobbler::Application*>(qApp)->currentConnection();
        if( connection && connection->state() != Stopped )
            ui.nav.nowScrobbling->setVisible( true );
        else
            ui.nav.nowScrobbling->setVisible( false );
    }
    centralWidget()->findChild<SideBySideLayout*>()->moveToWidget( w );
}

void
MetadataWindow::showProfile()
{
    setCurrentWidget( stack.profile );
}

void 
MetadataWindow::showNowScrobbling()
{
    setCurrentWidget( stack.nowScrobbling );
}

