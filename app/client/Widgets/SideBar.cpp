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

#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QShortcut>

#include <lastfm/User.h>

#include "lib/unicorn/widgets/AvatarWidget.h"

#include "../Application.h"
#include "SideBar.h"

QAbstractButton* newButton( const QString& text, QWidget* parent = 0 )
{
    QAbstractButton* pushButton = new QPushButton( parent );
    pushButton->setText( text );
    pushButton->setCheckable( true );
    pushButton->setAutoExclusive( true );
    pushButton->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );
    pushButton->setAttribute( Qt::WA_LayoutUsesWidgetRect );
    return pushButton;
}


SideBar::SideBar(QWidget *parent)
    :StylableWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    layout->addWidget( ui.nowPlaying = newButton( tr( "Now Playing" ), this ), Qt::AlignHCenter );
    ui.nowPlaying->setObjectName( "nowPlaying" );
    ui.nowPlaying->setChecked( true ); // the nowPlaying tab is always seleted at startUp
    layout->addWidget( ui.scrobbles = newButton( tr( "Scrobbles" ), this ), Qt::AlignHCenter);
    ui.scrobbles->setObjectName( "scrobbles" ); 
    layout->addWidget( ui.profile = newButton( tr( "Profile" ), this ), Qt::AlignHCenter);
    ui.profile->setObjectName( "profile" );
    layout->addWidget( ui.friends = newButton( tr( "Friends" ), this ), Qt::AlignHCenter);
    ui.friends->setObjectName( "friends" );
    layout->addWidget( ui.radio = newButton( tr( "Radio" ), this ), Qt::AlignHCenter);
    ui.radio->setObjectName( "radio" );
    layout->addStretch( 1 );

    connect( ui.nowPlaying, SIGNAL(clicked()), SLOT(onButtonClicked()));
    connect( ui.scrobbles, SIGNAL(clicked()), SLOT(onButtonClicked()));
    connect( ui.profile, SIGNAL(clicked()), SLOT(onButtonClicked()));
    connect( ui.friends, SIGNAL(clicked()), SLOT(onButtonClicked()));
    connect( ui.radio, SIGNAL(clicked()), SLOT(onButtonClicked()));

    //const QKeySequence & key, QWidget * parent, const char * member = 0, const char * ambiguousMember = 0, Qt::ShortcutContext context = Qt::WindowShortcut
    new QShortcut( Qt::CTRL + Qt::Key_BracketLeft, this, SLOT(onUp()));
    new QShortcut( Qt::CTRL + Qt::Key_BracketRight, this, SLOT(onDown()));
}


void
SideBar::onUp()
{
    if ( ui.nowPlaying->isChecked() ) ui.radio->click();
    else if ( ui.scrobbles->isChecked() ) ui.nowPlaying->click();
    else if ( ui.profile->isChecked() ) ui.scrobbles->click();
    else if ( ui.friends->isChecked() ) ui.profile->click();
    else if ( ui.radio->isChecked() ) ui.friends->click();
}

void
SideBar::onDown()
{
    if ( ui.nowPlaying->isChecked() ) ui.scrobbles->click();
    else if ( ui.scrobbles->isChecked() ) ui.profile->click();
    else if ( ui.profile->isChecked() ) ui.friends->click();
    else if ( ui.friends->isChecked() ) ui.radio->click();
    else if ( ui.radio->isChecked() ) ui.nowPlaying->click();
}

void
SideBar::click( int index )
{
    qobject_cast<QAbstractButton*>( layout()->itemAt( index )->widget() )->click();
}


void
SideBar::onButtonClicked()
{
    emit currentChanged( layout()->indexOf( qobject_cast<QWidget*>( sender() ) ) );
}
