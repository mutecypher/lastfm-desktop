
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

#include <QPalette>

#include "lib/unicorn/UnicornSettings.h"

#include "LoginPage.h"
#include "AccessPage.h"
#include "PluginsPage.h"
#include "BootstrapPage.h"
#include "TourMetadataPage.h"
#include "TourRadioPage.h"
#include "TourFinishPage.h"
#include "TourScrobblesPage.h"
#include "TourLocationPage.h"

#include "FirstRunWizard.h"

FirstRunWizard::FirstRunWizard( QWidget* parent )
    :QWizard( parent )
{
    setOption( QWizard::NoBackButtonOnStartPage, true );

    // We don't want the bowtie and the space on the left
    setWizardStyle( ClassicStyle );

    // get rid of the QWizardRuler
    m_palette = aApp->palette();
    QPalette palette = m_palette;
    palette.setColor( QPalette::Mid, QColor( 0, 0, 0, 0 ) );
    palette.setColor( QPalette::Base, QColor( 0, 0, 0, 0 ) );

    aApp->setPalette( palette );

    for ( int i = 0 ; i < QWizard::NButtons ; ++i )
    {
        QString objectName = QLatin1String("__qt__passive_wizardbutton");
        objectName += QString::number( i );
        button( static_cast<QWizard::WizardButton>( i ) )->setObjectName( objectName );
    }

    style()->polish( this );

    resize( 725, 460 );

    setPage( Page_Login, new LoginPage(this) );
    setPage( Page_Access, new AccessPage( this ) );
#ifdef Q_WS_WIN
    setPage( Page_Plugins, new PluginsPage() );
#endif
    setPage( Page_Bootstrap, new BootstrapPage( this ) );
    setPage( Page_Tour_Scrobbles, new TourScrobblesPage( this ) );
    setPage( Page_Tour_Metadata, new TourMetadataPage( this ) );
    setPage( Page_Tour_Radio, new TourRadioPage( this ) );
    setPage( Page_Tour_Finish, new TourFinishPage( this ) );
    setPage( Page_Tour_Location, new TourLocationPage( this ) );

    connect( this, SIGNAL( rejected() ), this, SLOT( onRejected() ) );
    connect( this, SIGNAL( accepted() ), this, SLOT( onWizardCompleted() ) );
}

int
FirstRunWizard::nextId() const
{
    switch ( currentId() )
    {
    case Page_Login:
        return Page_Access;
    case Page_Access:
#ifdef Q_OS_WIN32
        return Page_Plugins;
    case Page_Plugins:
        return Page_PluginsInstall:
    case Page_PluginsInstall
        if( aApp->currentSession() && aApp->currentSession()->userInfo().canBootstrap() )
            return Page_Bootstrap;
        else
            return Page_Tour_Scrobbles;
#elif defined Q_OS_MAC
        if( aApp->currentSession() && aApp->currentSession()->userInfo().canBootstrap() )
            return Page_Bootstrap;
        else
            return Page_Tour_Scrobbles;
#elif defined Q_WS_X11
        return Page_Tour_Scrobbles;
#endif
    case Page_Bootstrap:
        return Page_BootstrapProgress;
    case Page_Tour_Scrobbles:
        return Page_Tour_Metadata;
    case Page_Tour_Metadata:
        return Page_Tour_Radio;
    case Page_Tour_Radio:
        return Page_Tour_Location;
    case Page_Tour_Location:
        return Page_Tour_Finish;
    default:
        return -1;
    }
}

void
FirstRunWizard::onWizardCompleted()
{
    unicorn::Settings().setValue( "FirstRunWizardCompleted", true );

    aApp->setPalette( m_palette );
}


void
FirstRunWizard::onRejected()
{
    // if the user doesn't finish the wizard then we make sure
    // the settings are removed.
    QMap<QString, QString> lastSession = unicorn::Session::lastSessionData();
    if ( lastSession.contains( "username" ) )
    {
        unicorn::Settings us;
        us.remove( lastSession[ "username" ] );
    }
}
