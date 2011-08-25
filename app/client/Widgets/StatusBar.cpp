/*
   Copyright 2010 Last.fm Ltd.
      - Primarily authored by Jono Cole and Micahel Coffey

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

#include <QAction>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSizeGrip>

#include <Phonon/VolumeSlider>

#include "lib/unicorn/widgets/Label.h"
#include "lib/unicorn/StylableWidget.h"

#include "StatusBar.h"

#include "../Services/ScrobbleService.h"
#include "../Services/RadioService.h"
#include "../MediaDevices/DeviceScrobbler.h"
#include "../Application.h"

StatusBar::StatusBar( QWidget* parent )
    :QStatusBar( parent )
{
    addWidget( ui.widget = new StylableWidget( this) );
    QHBoxLayout* widgetLayout = new QHBoxLayout( ui.widget );
    widgetLayout->setContentsMargins( 0, 0, 0, 0 );
    widgetLayout->setSpacing( 0 );

    widgetLayout->addWidget( ui.cog = new QPushButton( this ), 0, Qt::AlignVCenter );
    ui.cog->setObjectName( "cog" );
    ui.cog->setAttribute( Qt::WA_LayoutUsesWidgetRect );

    widgetLayout->addWidget( ui.message = new Label( this ), 1, Qt::AlignVCenter );
    ui.message->setObjectName( "message" );
    ui.message->setAttribute( Qt::WA_LayoutUsesWidgetRect );
    ui.message->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );

    setStatus();

    addPermanentWidget( ui.permanentWidget = new StylableWidget( this ) );
    QHBoxLayout* permanentWidgetLayout = new QHBoxLayout( ui.permanentWidget );
    permanentWidgetLayout->setContentsMargins( 0, 0, 0, 0 );
    permanentWidgetLayout->setSpacing( 0 );

    permanentWidgetLayout->addWidget( ui.volMin = new QLabel( this ) );
    ui.volMin->setObjectName( "volMin" );
    ui.volMin->setAttribute( Qt::WA_LayoutUsesWidgetRect );
    permanentWidgetLayout->addWidget( ui.volumeSlider = new Phonon::VolumeSlider( RadioService::instance().audioOutput(), this ) );
    ui.volumeSlider->setOrientation( Qt::Horizontal );
    permanentWidgetLayout->addWidget( ui.volMax = new QLabel( this ) );
    ui.volMax->setObjectName( "volMax" );
    ui.volMax->setAttribute( Qt::WA_LayoutUsesWidgetRect );

    bool scrobblingOn = unicorn::UserSettings().value( "scrobblingOn", true ).toBool();
    permanentWidgetLayout->addWidget( ui.scrobbleToggle = new QPushButton( this ) );
    ui.scrobbleToggle->setObjectName( "scrobbleToggle" );
    ui.scrobbleToggle->setCheckable( true );
    ui.scrobbleToggle->setChecked( scrobblingOn );
    onScrobbleToggled( scrobblingOn );
    ui.scrobbleToggle->setAttribute( Qt::WA_LayoutUsesWidgetRect );

    aApp->scrobbleToggleAction()->setChecked( scrobblingOn );

    connect( aApp->scrobbleToggleAction(), SIGNAL(toggled(bool)), SLOT(onScrobbleToggled(bool)) );

    connect( ui.scrobbleToggle, SIGNAL(toggled(bool)), SLOT(onScrobbleToggled(bool)) );

    aApp->isInternetConnectionUp() ? onConnectionUp() : onConnectionDown();

    connect( aApp, SIGNAL( internetConnectionDown() ), SLOT( onConnectionDown() ) );
    connect( aApp, SIGNAL( internetConnectionUp() ), SLOT( onConnectionUp() ) );

    connect( aApp, SIGNAL( gotUserInfo(lastfm::UserDetails)), SLOT( onGotUserInfo(lastfm::UserDetails) ) );

    DeviceScrobbler* deviceScrobbler = ScrobbleService::instance().deviceScrobbler();
    if( deviceScrobbler )
    {
        connect( deviceScrobbler, SIGNAL( detectedIPod( QString )), SLOT( onIPodDetected( QString )));
        connect( deviceScrobbler, SIGNAL( processingScrobbles()), SLOT( onProcessingScrobbles()));
        connect( deviceScrobbler, SIGNAL( foundScrobbles( QList<lastfm::Track> )), SLOT( onFoundScrobbles( QList<lastfm::Track> )));
        connect( deviceScrobbler, SIGNAL( noScrobblesFound()),SLOT( onNoScrobblesFound()));
    }

    connect( this, SIGNAL(messageChanged(QString)), SLOT(onMessagedChanged(QString)));

    connect( aApp, SIGNAL(sessionChanged(unicorn::Session*)), SLOT(onSessionChanged(unicorn::Session*)));
}


void
StatusBar::onSessionChanged( unicorn::Session* session )
{
    bool scrobblingOn = unicorn::UserSettings( session->userInfo() ).value( "scrobblingOn", true ).toBool();
    onScrobbleToggled( scrobblingOn );
}


void
StatusBar::onScrobbleToggled( bool scrobblingOn )
{
    unicorn::UserSettings().setValue( "scrobblingOn", scrobblingOn );
    ScrobbleService::instance().setScrobblingOn( scrobblingOn );
    ui.scrobbleToggle->setChecked( scrobblingOn );
    ui.scrobbleToggle->setToolTip( scrobblingOn ? tr( "Scrobbling on" ) : tr( "Scrobbling off" ) );
    aApp->scrobbleToggleAction()->setChecked( scrobblingOn );
}

void
StatusBar::setSizeGripVisible( bool visible )
{
    ui.sizeGrip->setVisible( visible );
}

void
StatusBar::onMessagedChanged( const QString& message )
{
    if ( message.isEmpty() )
        setStatus();
}

void
StatusBar::setStatus()
{
    ui.message->setText( tr("%1 (%2)").arg( lastfm::ws::Username, m_online ? tr( "Online" ) : tr( "Offline" ) ));
}

void
StatusBar::onGotUserInfo( lastfm::UserDetails /*userDetails*/ )
{
    setStatus();
}

void
StatusBar::onConnectionUp()
{
    m_online = true;
    setStatus();
}

void
StatusBar::onConnectionDown()
{
    m_online = false;
    setStatus();
}

void
StatusBar::onIPodDetected( QString iPod )
{
    //showMessage( tr("iPod Detected... ") + iPod, 3 * 1000 );
}

void
StatusBar::onProcessingScrobbles()
{
    //showMessage( tr("Processing iPod Scrobbles..."), 3 * 1000 );
}

void
StatusBar::onFoundScrobbles( QList<lastfm::Track> tracks )
{
//    tracks.count() == 1 ?
//        showMessage( tr("%1 scrobble found").arg( tracks.count() ), 10 * 1000 ):
//        showMessage( tr("%1 scrobbles found").arg( tracks.count() ), 10 * 1000 );
}

void
StatusBar::onNoScrobblesFound()
{
    //showMessage( tr("No scrobbles found"), 10 * 1000 );
}


