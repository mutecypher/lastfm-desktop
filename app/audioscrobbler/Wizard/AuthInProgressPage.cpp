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

#include "AuthInProgressPage.h"
#include <QVBoxLayout>
#include <QLabel>
#include "lib/unicorn/LoginProcess.h"
#include "lib/unicorn/UnicornSession.h"

AuthInProgressPage::AuthInProgressPage( QWizard* parent )
                   :QWizardPage( parent ),
                    m_loginProcess( 0 )
{
    setTitle( tr( "Hmm, we can't tell if you've connected yet." ));
    
    QVBoxLayout* layout = new QVBoxLayout( this );
    
    QLabel* description = new QLabel( tr( "If you haven't connected to Last.fm yet (or you clicked cancel) please try again now by opening this link in your browser and clicking <strong>'Yes, I accept'</strong>:" ));
    description->setWordWrap( true );

    QLabel* description2 = new QLabel( tr( "If you have already done this, click continue." ));
    description2->setWordWrap( true );


    layout->addWidget( description );
    layout->addWidget( description2 );
}

void
AuthInProgressPage::initializePage()
{
    if ( m_loginProcess )
    {
        delete m_loginProcess;
    }
    m_loginProcess = new unicorn::LoginProcess( this );
    connect( m_loginProcess, SIGNAL( gotSession( unicorn::Session* ) ),
             this, SLOT( onAuthenticated( unicorn::Session* ) ) );

    m_loginProcess->authenticate();

    //ui.continueMsg->show();
    //ui.browserMsg->show();
    //ui.loginUrl->setText( m_loginProcess->authUrl().toString() );
    //ui.loginUrl->home( false );
    //ui.loginUrl->show();
}


void 
AuthInProgressPage::onAuthenticated( unicorn::Session* session )
{
    if( session )
    {
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
