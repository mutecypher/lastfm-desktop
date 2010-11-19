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

LoginPage::LoginPage( QWidget* parent )
          :QWizardPage( parent )
          , m_loginProcess( 0 )
          , m_isComplete( false )
{
    setCommitPage( true );

    setTitle( tr( "Connect with Last.fm" ) );
    QVBoxLayout* pageLayout = new QVBoxLayout( this );
    
    QVBoxLayout* loginLayout = new QVBoxLayout;
    ui.description = new QLabel( tr( "If you already have a Last.fm account, connect with Last.fm.\n\n" ) +
                                 tr( "If you don't have a Last.fm account, you can sign up now for free." ) );
    
    ui.description->setObjectName( "description" );
    ui.description->setWordWrap( true );
    
    ui.continueMsg = new QLabel( tr( "Once you have approved this app, click Next to complete the login process." ) );
    ui.continueMsg->setObjectName( "continueMsg" );
    ui.continueMsg->setWordWrap( true );
    ui.continueMsg->hide();
    
    ui.browserMsg = new QLabel( tr( "If your browser doesn't open automatically, "
                                    "you can visit the following URL:" ) );
    
    ui.browserMsg->setObjectName( "browserMsg" );
    ui.browserMsg->setWordWrap( true );
    ui.browserMsg->hide();
    
    ui.loginUrl = new QLineEdit;
    ui.loginUrl->setReadOnly( true );
    ui.loginUrl->hide();
    ui.loginUrl->setObjectName( "loginUrl" );
    
    
    QHBoxLayout* buttonLayout = new QHBoxLayout;

    //buttonLayout->addWidget( ui.okButton );
    //buttonLayout->addStretch( 1 );

    loginLayout->addWidget( ui.description );
    loginLayout->addLayout( buttonLayout );
    loginLayout->addSpacing( 10 );
    loginLayout->addWidget( ui.continueMsg );
    loginLayout->addSpacing( 10 );
    loginLayout->addWidget( ui.browserMsg );
    loginLayout->addWidget( ui.loginUrl );
    
    pageLayout->addLayout( loginLayout );
}


void
LoginPage::initializePage()
{
    wizard()->setOption( QWizard::HaveCustomButton1, true );
    setButtonText( QWizard::CommitButton, tr( "Connect with your Last.fm account" ) );
    setButtonText( QWizard::CustomButton1, tr( "Sign up" ));
    m_isComplete = false;
//    ui.okButton->setEnabled( true );
}


void 
LoginPage::cleanupPage()
{
    m_isComplete = false;
    //ui.okButton->setEnabled( true );
    ui.continueMsg->hide();
    ui.browserMsg->hide();
    ui.loginUrl->hide();
}

void 
LoginPage::authenticate() 
{
    //ui.okButton->setEnabled( false );
    if ( m_loginProcess )
    {
        delete m_loginProcess;
    }
    m_loginProcess = new unicorn::LoginProcess( this );
    connect( m_loginProcess, SIGNAL( gotSession( unicorn::Session* ) ),
             this, SLOT( onAuthenticated( unicorn::Session* ) ) );

    m_loginProcess->authenticate();

    ui.continueMsg->show();
    ui.browserMsg->show();
    ui.loginUrl->setText( m_loginProcess->authUrl().toString() );
    ui.loginUrl->home( false );
    ui.loginUrl->show();
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
LoginPage::onAuthenticated( unicorn::Session* session )
{
    if ( session )
    {
        m_isComplete = true;
        emit completeChanged();
        wizard()->next();
        wizard()->showNormal();
        wizard()->setFocus();
        wizard()->raise();
        wizard()->activateWindow();
    }
    else
    {
        m_loginProcess->showError();
    }
}
