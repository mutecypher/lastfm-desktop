/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Jono Cole

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

#include <QLabel>
#include <QVBoxLayout>

#include "lib/unicorn/UnicornSession.h"

#include "../Application.h"
#include "WelcomeWidget.h"

WelcomeWidget::WelcomeWidget( QWidget* p )
           :StylableWidget( p )
{
    QVBoxLayout* layout = new QVBoxLayout( this);

    QWidget* welcomeBox = new StylableWidget();
    {
        QHBoxLayout* h1 = new QHBoxLayout( welcomeBox );

        h1->addWidget( ui.as = new QLabel( "", this ) );
        ui.as->setObjectName("asLogo");

        QVBoxLayout* v1 = new QVBoxLayout();

        v1->addWidget( ui.welcome = new QLabel( "", this ) );
        ui.welcome->setObjectName( "welcome" );

        v1->addWidget( ui.callToAction = new QLabel( tr("Listen to some music to start scrobbling and update your Last.fm profile."), this ) );
        ui.callToAction->setObjectName( "callToAction" );

        v1->addStretch( 1 );
        h1->addLayout( v1 );
        h1->addStretch( 1 );
    }

    layout->addWidget( welcomeBox );
    layout->addStretch( 1 );
    
    //On first run we won't catch the sessionChanged signal on time
    //so we should try to get the current session from the unicorn::Application
    unicorn::Session* currentSession = aApp->currentSession();
    if ( currentSession )
        onSessionChanged( currentSession );

    connect( aApp, SIGNAL( sessionChanged( unicorn::Session* ) ), SLOT( onSessionChanged( unicorn::Session* ) ) );
}

void
WelcomeWidget::onSessionChanged( unicorn::Session* session )
{
    if ( !session )
        return;

    ui.welcome->setText( tr( "Hi, %1." ).arg( session->userInfo().name() ) );
}

