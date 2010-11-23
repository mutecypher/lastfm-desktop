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
#include "LoginPage.h"
#include "../Application.h"

#include "lib/unicorn/LoginProcess.h"
#include "lib/unicorn/UnicornSession.h"

#ifdef WIN32
#include <windows.h>
#endif // WIN32

#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDesktopServices>

LoginPage::LoginPage( QWidget* parent )
          :QWizardPage( parent )
          , m_loginProcess( 0 )
          , m_isComplete( false )
{
//    setCommitPage( true );

    setTitle( tr( "Connect with Last.fm" ) );
    QVBoxLayout* pageLayout = new QVBoxLayout( this );
    
    ui.description = new QLabel( tr( "If you already have a Last.fm account, connect with Last.fm.\n\n" ) +
                                 tr( "If you don't have a Last.fm account, you can sign up now for free." ) );
    
    ui.description->setObjectName( "description" );
    ui.description->setWordWrap( true );
    
    pageLayout->addWidget( ui.description );
}


void
LoginPage::initializePage()
{
    wizard()->setOption( QWizard::HaveCustomButton1, true );
    connect( wizard()->button( QWizard::CustomButton1 ), SIGNAL( clicked()), SLOT( onSignUpClicked()));
    setButtonText( QWizard::NextButton, tr( "Connect with your Last.fm account" ) );
    setButtonText( QWizard::CustomButton1, tr( "Sign up" ));
    m_isComplete = false;
}


void 
LoginPage::cleanupPage()
{
    disconnect( wizard()->button( QWizard::CustomButton1 ), SIGNAL( clicked()), this, 0 );
    m_isComplete = false;
}

bool 
LoginPage::validatePage()
{
    wizard()->setOption( QWizard::HaveCustomButton1, false );
    return true;
}

bool
LoginPage::isComplete() const
{
    return true;
}

void 
LoginPage::onSignUpClicked()
{
    QDesktopServices::openUrl( QUrl( "http://www.last.fm/join" ));
}
