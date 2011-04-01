/*
   Copyright 2010 Last.fm Ltd.
      - Primarily authored by Jono Cole and Micahel Coffey

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

#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

#include "lib/unicorn/widgets/GhostWidget.h"

#include "TitleBar.h"

#include "../Application.h"

TitleBar::TitleBar( QWidget* parent )
    :StylableWidget( parent )
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    QPushButton* pb = new QPushButton( "Close" );
    pb->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    connect( pb, SIGNAL(clicked()), SIGNAL( closeClicked()));

#ifdef Q_OS_MAC
    pb->setShortcut( Qt::CTRL + Qt::Key_H );
    layout->addWidget( pb );
#else
    GhostWidget* ghost = new GhostWidget( this );
    ghost->setOrigin( pb );
    layout->addWidget( ghost );
#endif

    layout->addStretch( 1 );
    QLabel* title = new QLabel( tr("Last.fm Scrobbler"), this );
    title->setMargin( 0 );
    layout->addWidget( title, 0 );
    layout->addStretch( 1 );

#ifndef Q_OS_MAC
    layout->addWidget( pb );
#else
    GhostWidget* ghost = new GhostWidget( this );
    ghost->setOrigin( pb );
    layout->addWidget( ghost );
#endif
}

