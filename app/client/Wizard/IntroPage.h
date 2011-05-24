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
#ifndef INTRO_PAGE_H_
#define INTRO_PAGE_H_

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDebug>

#include "lib/unicorn/StylableWidget.h"
#include "lib/unicorn/layouts/SideBySideLayout.h"

#include <QWizardPage>

class IntroPage : public QWizardPage
{
    Q_OBJECT
public:
    IntroPage( QWidget* parent = 0 )
    :QWizardPage( parent )
    {
        setTitle(tr( "Welcome" ));
#ifndef Q_OS_MAC
        setSubTitle(tr( "Welcome to the Last.fm client configuration wizard" ));
#endif
        new QVBoxLayout( this );
        setContentsMargins( 0, 0, 0, 0 );
        layout()->setSpacing( 0 );

        QWidget* welcome = new QWidget( this );
        new QVBoxLayout( welcome );

        welcome->setContentsMargins( 0, 0, 0, 0 );
        welcome->layout()->setSpacing( 0 );
        
        QLabel* welcomeLabel = new QLabel( tr( 
#ifdef Q_OS_MAC
            "Welcome to the Last.fm client configuration wizard\n\n"
#endif
            "Last.fm will now look for music players on your computer "
            "and then download the plugins you need to get scrobbling.\n\n"
            "Before continuing, make sure all your music player software is closed."));
        welcomeLabel->setWordWrap( true );
        welcome->layout()->addWidget( welcomeLabel );
        static_cast<QBoxLayout*>(welcome->layout())->addStretch();

        layout()->addWidget( welcome );
    }

};

#endif //INTRO_PAGE_H_
