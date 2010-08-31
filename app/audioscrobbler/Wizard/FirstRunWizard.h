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
#ifndef FIRST_RUN_WIZARD_H_
#define FIRST_RUN_WIZARD_H_

#include "IntroPage.h"
#include "LoginPage.h"
#include "PluginPage.h"
#include "BootstrapPage.h"
#include "WelcomePage.h"

#include "lib/unicorn/UnicornSettings.h"
#include <QWizard>
#include <QDebug>

/**  @author Jono Cole <jono@last.fm>
  *  @brief Initial wizard to guide the user through login, plugin installation, bootstrapping etc 
  */
class FirstRunWizard : public QWizard
{
    Q_OBJECT
    enum {
       Page_Intro = 0,
       Page_Login,
       Page_Plugin,
       Page_Bootstrap,
       Page_Welcome
    };

public:
    FirstRunWizard( QWidget* parent = 0 )
    : QWizard( parent )
    {
        resize( 625, 440 );
        setPage( Page_Intro, new IntroPage(this));
        #ifdef Q_WS_MAC || Q_WS_WIN
        setPage( Page_Plugin, new PluginPage());
        #endif
        setPage( Page_Login, new LoginPage(this));
        setPage( Page_Bootstrap, new BootstrapPage( this ));
        setPage( Page_Welcome, new WelcomePage( this ) );
        connect( this, SIGNAL( rejected() ), this, SLOT( onRejected() ) );
        connect( this, SIGNAL( accepted() ), this, SLOT( onWizardCompleted() ) );
    }

    int nextId() const
    {
        switch( currentId()) {
            case Page_Intro:
                return Page_Login;
            
            case Page_Login:
            #ifdef Q_OS_WIN32
                return Page_Plugin;
            case Page_Plugin:
                return Page_Welcome;
            #elif Q_WS_MAC
                return Page_Bootstrap;
            case Page_Bootstrap:
                return Page_Welcome;
            #else Q_WS_X11
                return Page_Welcome;
            #endif

            default:
                return -1;
        }
    }

private slots:
    void onWizardCompleted()
    {
        unicorn::Settings().setValue( "FirstRunWizardCompleted", true );
    }


    void onRejected()
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

};

#endif //FIRST_RUN_WIZARD_H_
