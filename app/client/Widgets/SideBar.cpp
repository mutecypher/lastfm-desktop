/*
   Copyright 2010 Last.fm Ltd.
      - Primarily authored by Jono Cole and Michael Coffey

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

#include <QRadioButton>
#include <QVBoxLayout>

#include "../Application.h"
#include "SideBar.h"

SideBar::SideBar(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    layout->addWidget( ui.np = new QRadioButton( tr( "Now Playing" ), this ));
    layout->addWidget( ui.s = new QRadioButton( tr( "Scrobbles" ), this ));
    layout->addWidget( ui.r = new QRadioButton( tr( "Radio" ), this ));
    layout->addStretch( 1 );

    connect( ui.np, SIGNAL(clicked()), SLOT(onButtonClicked()));
    connect( ui.s, SIGNAL(clicked()), SLOT(onButtonClicked()));
    connect( ui.r, SIGNAL(clicked()), SLOT(onButtonClicked()));

    connect( aApp, SIGNAL(sessionChanged(unicorn::Session*)), SLOT(onSessionChanged(unicorn::Session*)));
}

void
SideBar::onButtonClicked()
{
    emit currentChanged( layout()->indexOf( qobject_cast<QRadioButton*>( sender() ) ) );
}


void
SideBar::onSessionChanged( unicorn::Session* newSession )
{
    QUrl imageUrl = newSession->userInfo().imageUrl( lastfm::ImageSize::Medium, true );
    connect( lastfm::nam()->get( QNetworkRequest( imageUrl ) ), SIGNAL(finished()), SLOT(onGotAvatar()));
}

void
SideBar::onGotAvatar()
{
    QPixmap avatar;
    avatar.loadFromData( qobject_cast<QNetworkReply*>(sender())->readAll() );
    ui.np->setIcon( QIcon( avatar ) );
}
