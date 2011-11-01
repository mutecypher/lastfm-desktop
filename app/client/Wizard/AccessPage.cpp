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

#include <QVBoxLayout>
#include <QLabel>
#include <QStyle>
#include <QAbstractButton>

#include "lib/unicorn/LoginProcess.h"
#include "lib/unicorn/UnicornSession.h"

#include "../Application.h"

#include "FirstRunWizard.h"

#include "AccessPage.h"

AccessPage::AccessPage()
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 20 );
    
    layout->addWidget( ui.image = new QLabel(), 0, Qt::AlignTop | Qt::AlignHCenter );
    ui.image->setObjectName( "image" );

    layout->addWidget( ui.description = new QLabel( tr( "<p>Please click the <strong>Yes, Allow Access</strong> button in your web browser to connect your Last.fm account to the Last.fm Desktop App.</p>"
                                                        "<p>If you haven't connected because you closed the browser window or you clicked cancel, please try again.<p/>" )),
                       0, Qt::AlignTop);
    ui.description->setObjectName( "description" );
    ui.description->setWordWrap( true );
}

void
AccessPage::initializePage()
{
    setTitle( tr( "We're waiting for you to connect to Last.fm" ));

    wizard()->setCommitPage( true );

    if ( wizard()->canGoBack() )
        wizard()->setButton( FirstRunWizard::BackButton, tr( "<< Back" ) );
    wizard()->setButton( FirstRunWizard::NextButton, tr( "Continue" ) );
    QAbstractButton* custom = wizard()->setButton( FirstRunWizard::CustomButton, tr( "Try Again" ) );

    connect( custom, SIGNAL(clicked()), SLOT(tryAgain()));

    tryAgain();
}

void
AccessPage::tryAgain()
{
    unicorn::LoginProcess* loginProcess = new unicorn::LoginProcess( this );
    m_loginProcesses << loginProcess;
    connect( loginProcess, SIGNAL( gotSession( unicorn::Session* ) ), SLOT( onAuthenticated( unicorn::Session* ) ) );
    loginProcess->authenticate();
}

void
AccessPage::onAuthenticated( unicorn::Session* session )
{
    if ( session )
    {
        // Wait to find out they're details such as canBootstrap, subscriber, etc
        connect( aApp, SIGNAL(gotUserInfo(lastfm::User)), SLOT(onGotUserInfo(lastfm::User)));
    }
    else
    {
        qobject_cast<unicorn::LoginProcess*>(sender())->showError();
    }
}

void
AccessPage::onGotUserInfo( const lastfm::User& user )
{
    // make sure the wizard is shown again after they allow access on the website.
    wizard()->showWelcome();
    wizard()->next();
    wizard()->showNormal();
    wizard()->setFocus();
    wizard()->raise();
    wizard()->activateWindow();

    foreach ( unicorn::LoginProcess* loginProcess, m_loginProcesses )
        loginProcess->deleteLater();
}

void
AccessPage::cleanupPage()
{
}


bool
AccessPage::validatePage()
{
    if ( aApp->currentSession() )
        return true;

    // There is no session so try to fetch it
    // onAuthenticated will be called if we find one
    // just try with the most recent one
    unicorn::LoginProcess* loginProcess = m_loginProcesses[ m_loginProcesses.count() - 1 ];

    loginProcess->getSession( loginProcess->token() );
    return false;
}

