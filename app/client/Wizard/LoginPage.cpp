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

#include "FirstRunWizard.h"
#include "LoginPage.h"
#include "../Application.h"

#include <lastfm/UrlBuilder.h>

#include "lib/unicorn/LoginProcess.h"
#include "lib/unicorn/UnicornSession.h"

#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDesktopServices>
#include <QStyle>

LoginPage::LoginPage()
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 20 );
    
    layout->addWidget( ui.image = new QLabel( this ), 0, Qt::AlignTop | Qt::AlignHCenter );
    ui.image->setObjectName( "image" );

    layout->addWidget( ui.description = new QLabel( tr( "<p>Already a Last.fm user? You can connect the Last.fm Desktop App to your profile and keep a record of the music you listen to.</p>"
                                                        "<p>If you don't have an account you can sign up now for free.</p>" ) ),
                       0,
                       Qt::AlignTop );
    
    ui.description->setObjectName( "description" );
    ui.description->setWordWrap( true );
}


void
LoginPage::initializePage()
{
    setTitle( tr( "Hello! Let's get started by connecting your Last.fm account" ) );

    wizard()->setButton( FirstRunWizard::NextButton, tr( "Connect Your Account" ) );
    QAbstractButton* custom = wizard()->setButton( FirstRunWizard::CustomButton, tr( "Sign up" ) );

    connect( custom, SIGNAL(clicked()), SLOT(onSignUpClicked()));
}


void
LoginPage::cleanupPage()
{
}

void 
LoginPage::onSignUpClicked()
{
    QDesktopServices::openUrl( lastfm::UrlBuilder( "join" ).url() );
}
