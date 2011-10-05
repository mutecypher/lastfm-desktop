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

#include "lib/unicorn/LoginProcess.h"
#include "lib/unicorn/UnicornSession.h"

#include "../Application.h"

#include "AccessPage.h"

AccessPage::AccessPage( QWizard* parent )
    :QWizardPage( parent )
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );
    
    layout->addWidget( ui.image = new QLabel(), 0, Qt::AlignCenter );
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

    wizard()->style()->polish( this );

    delete m_loginProcess;

    setCommitPage( true );

    setButtonText( QWizard::CommitButton, tr( "Continue" ) );

    m_loginProcess = new unicorn::LoginProcess( this );
    connect( m_loginProcess, SIGNAL( gotSession( unicorn::Session* ) ), SLOT( onAuthenticated( unicorn::Session* ) ) );
    m_loginProcess->authenticate();
}

void
AccessPage::onAuthenticated( unicorn::Session* session )
{
    if ( session )
    {
        // make sure the wizard is shown again after they allow access on the website.
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

void
AccessPage::cleanupPage()
{
    delete m_loginProcess;
}


bool
AccessPage::validatePage()
{
    if ( aApp->currentSession() )
        return true;

    // There is no session so try to fetch it
    // onAuthenticated will be called if we find one
    m_loginProcess->getSession( m_loginProcess->token() );
    return false;
}

