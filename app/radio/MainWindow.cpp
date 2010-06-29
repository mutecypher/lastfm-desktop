/*
   Copyright 2005-2009 Last.fm Ltd. 

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
#include "MainWidget.h"
#include "widgets/MultiStarterWidget.h"
#include "widgets/PlaybackControlsWidget.h"
#include "widgets/NowPlayingWidget.h"
#include "lib/unicorn/UnicornApplication.h"
#include "lib/unicorn/qtwin.h"
#include "lib/unicorn/AnimatedStatusBar.h"
#include "lib/unicorn/widgets/MessageBar.h"
#include "lib/unicorn/StylableWidget.h"
#include <lastfm/RadioStation>
#include <QLineEdit>
#include <QSizeGrip>
#include <QMenuBar>
#include <QPaintEngine>
#include <QPainter>
#include "Radio.h"

MainWindow::MainWindow()
{
    setUnifiedTitleAndToolBarOnMac( true );
    QtWin::extendFrameIntoClientArea( this );

    QStatusBar* status = new QStatusBar( this );

    addDragHandleWidget( status );
    PlaybackControlsWidget* pcw = new PlaybackControlsWidget( status );

    status->setSizeGripEnabled( false );

    status->addWidget( pcw, 1 );
    setStatusBar( status );

    QWidget* w = new StylableWidget();
    
    setCentralWidget( m_mainWidget = new MainWidget() );

    layout()->setSizeConstraint( QLayout::SetFixedSize );

    m_messageBar = new MessageBar( w );

    connect(m_mainWidget, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));

    connect(pcw, SIGNAL(startRadio(RadioStation)), m_mainWidget, SLOT(onStartRadio(RadioStation)));
    
 //   connect(radio, SIGNAL(stopped()), status, SLOT(hideAnimated()));
 //   connect(radio, SIGNAL(tuningIn( const RadioStation&)), status, SLOT(showAnimated()));
    
    connect( qApp, SIGNAL( sessionChanged( const unicorn::Session&, const unicorn::Session& ) ), 
             SLOT( onSessionChanged( const unicorn::Session&, const unicorn::Session& ) ) );
    connect( qApp, SIGNAL( sessionChanged( const unicorn::Session&, const unicorn::Session& ) ), 
             m_mainWidget, SIGNAL( sessionChanged( const unicorn::Session&, const unicorn::Session& ) ) );
    //if we've got this far we must already have a session so use
    //the current session to start things rolling.
    onSessionChanged( qobject_cast<unicorn::Application*>(qApp)->currentSession(), Session());


    finishUi();

#ifndef NDEBUG
    menuBar()->addMenu("Normania")->addAction( tr("RQL"), m_mainWidget, SLOT(rawrql()), QKeySequence(tr("Ctrl+r")) );
#endif
}

void
MainWindow::addWinThumbBarButtons( QList<QAction*>& thumbButtonActions )
{
//    QAction* love = new QAction( "Love" , this );
//    love->setCheckable( true );
//    QIcon loveIcon;
//    loveIcon.addFile( ":/love-rest.png", QSize( 16, 16), QIcon::Normal, QIcon::On );
//    loveIcon.addFile( ":/love-isloved.png", QSize( 16, 16), QIcon::Normal, QIcon::Off );
//    love->setIcon( loveIcon );
//    thumbButtonActions.append( love );
}

void 
MainWindow::onSessionChanged( const unicorn::Session& s, const unicorn::Session& )
{
    qDebug() << "session changed and the app noticed it!";
    User user;
    qDebug() << "fetching friends and recent stations for" << user;
    connect(user.getFriends(), SIGNAL(finished()), m_mainWidget, SLOT(onUserGotFriends()));
    connect(user.getRecentStations(), SIGNAL(finished()), m_mainWidget, SLOT(onUserGotRecentStations()));
}

void
MainWindow::onRadioError(int code, const QVariant& data)
{
    switch (code)
    {
        case lastfm::ws::NotEnoughContent:
            m_messageBar->show(tr("Sorry, there is no more content available for this radio station."));
            break;
            
        case lastfm::ws::MalformedResponse:
        case lastfm::ws::TryAgainLater:
            m_messageBar->show(tr("Sorry, there was a radio error at Last.fm. Please try again later."));
            break;
            
        case lastfm::ws::SubscribersOnly:
            m_messageBar->show(tr("Sorry, this station is only available to Last.fm subscribers. "
                           "<A href='http://www.last.fm/subscribe'>Sign up</a>."));
            break;

        case lastfm::ws::UnknownError:
            // string contains Phonon generated user readable error message
            m_messageBar->show(data.toString());
            break;

        default:
            m_messageBar->show("Sorry, an unexpected error occurred.");
            break;
    }
}
