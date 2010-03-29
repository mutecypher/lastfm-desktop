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

#include "RestWidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "lib/unicorn/UnicornApplication.h"
#include "lib/unicorn/UnicornSession.h"

#include <lastfm/ws.h>

using unicorn::Session;
RestWidget::RestWidget( QWidget* p )
           :StylableWidget( p )
{
    setAutoFillBackground( true );
    QPushButton* b;
    QVBoxLayout* l = new QVBoxLayout( this );
    l->addWidget( ui.welcomeLabel = new QLabel( tr("Hi, %1.\nListen to some music to scrobble it to your Last.fm profile." ).arg( lastfm::ws::Username )), 0, Qt::AlignBottom);
    l->addWidget( b = new QPushButton( tr( "Switch users?" )), 0, Qt::AlignTop);
    b->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    
    connect( qApp, SIGNAL(sessionChanged(unicorn::Session, unicorn::Session)), SLOT(onSessionChanged(unicorn::Session)));
    connect( b, SIGNAL(clicked()), qApp, SLOT( manageUsers()));
    b->setCursor( Qt::PointingHandCursor );
}

void 
RestWidget::onSessionChanged( const Session& session )
{
    ui.welcomeLabel->setText( tr("Hi, %1.\nListen to some music to scrobble it to your Last.fm profile." ).arg( session.username() ));
}
