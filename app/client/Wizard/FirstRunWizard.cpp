
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

#include "lib/unicorn/UnicornSettings.h"

#include "IntroPage.h"
#include "LoginPage.h"
#include "AuthInProgressPage.h"
#include "PluginPage.h"
#include "BootstrapPage.h"
#include "BootstrapInProgressPage.h"
#include "WelcomePage.h"
#include "SystemTrayPage.h"

#include "FirstRunWizard.h"

FirstRunWizard::FirstRunWizard( QWidget* parent )
    :QWizard( parent )
{
    setOption( QWizard::NoBackButtonOnStartPage, true );

    // We don't want the bowtie and the space on the left
    setWizardStyle( ClassicStyle );

    resize( 725, 460 );

    setPage( Page_Login, new LoginPage(this));
    setPage( Page_AuthInProgress, new AuthInProgressPage( this ));
#if defined(Q_WS_MAC) || defined(Q_WS_WIN)
    setPage( Page_Plugin, new PluginPage());
#endif
    setPage( Page_Bootstrap, new BootstrapPage( this ));
    // This wizard page needs the scrobble service, so don't create it until we have an account
    //setPage( Page_BootstrapInProgress, new BootstrapInProgressPage( this ));
    setPage( Page_Welcome, new WelcomePage( this ) );
    setPage( Page_SystemTray, new SystemTrayPage( this ));

    connect( this, SIGNAL( rejected() ), this, SLOT( onRejected() ) );
    connect( this, SIGNAL( accepted() ), this, SLOT( onWizardCompleted() ) );
}

int
FirstRunWizard::nextId() const
{
    switch ( currentId() )
    {
    case Page_Login:
        return Page_AuthInProgress;

    case Page_AuthInProgress:
#if defined(Q_WS_WIN) || defined(Q_WS_MAC)
        return Page_Plugin;

    case Page_Plugin:
        if( aApp->currentSession() && aApp->currentSession()->userInfo().canBootstrap() )
            return Page_Bootstrap;
        else
            return Page_Welcome;

    case Page_Bootstrap:
        if( !field( "bootstrap_player" ).toString().isEmpty() )
            return Page_BootstrapInProgress;
        else
            return Page_Welcome;

    case Page_BootstrapInProgress:
        return Page_Welcome;
#elif defined Q_WS_X11
        return Page_Welcome;
#endif

    case Page_Welcome:
        return Page_SystemTray;

    default:
        return -1;
    }
}

void
FirstRunWizard::onWizardCompleted()
{
    unicorn::Settings().setValue( "FirstRunWizardCompleted", true );
}


void
FirstRunWizard::onRejected()
{
    //if the user doesn't finish the wizard then we make sure
    //the settings are removed.
    QMap<QString, QString> lastSession = unicorn::Session::lastSessionData();
    if ( lastSession.contains( "username" ) )
    {
        unicorn::Settings us;
        us.remove( lastSession[ "username" ] );
    }
}
