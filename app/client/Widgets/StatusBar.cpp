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

#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSizeGrip>

#include "StatusBar.h"

#include "../Services/ScrobbleService.h"
#include "../MediaDevices/DeviceScrobbler.h"
#include "../Application.h"

StatusBar::StatusBar( QWidget* parent )
    :QStatusBar( parent ), m_scrobbleCount(-1)
{
    setStatus();
    addPermanentWidget( m_inetStatus = new QLabel( tr(""), this ) );

    aApp->isInternetConnectionUp() ? onConnectionUp() : onConnectionDown();

    connect( aApp, SIGNAL( internetConnectionDown() ), SLOT( onConnectionDown() ) );
    connect( aApp, SIGNAL( internetConnectionUp() ), SLOT( onConnectionUp() ) );

    connect( aApp, SIGNAL( gotUserInfo(lastfm::UserDetails)), SLOT( onGotUserInfo(lastfm::UserDetails) ) );

    DeviceScrobbler* deviceScrobbler = ScrobbleService::instance().deviceScrobbler();
    if( deviceScrobbler ) {
        connect( deviceScrobbler, SIGNAL( detectedIPod( QString )), SLOT( onIPodDetected( QString )));
        connect( deviceScrobbler, SIGNAL( processingScrobbles()), SLOT( onProcessingScrobbles()));
        connect( deviceScrobbler, SIGNAL( foundScrobbles( QList<lastfm::Track> )), SLOT( onFoundScrobbles( QList<lastfm::Track> )));
        connect( deviceScrobbler, SIGNAL( noScrobblesFound()),SLOT( onNoScrobblesFound()));
    }

    connect( &ScrobbleService::instance(), SIGNAL(scrobblesCached(QList<lastfm::Track>)), SLOT(onScrobblesCached(QList<lastfm::Track>)));
}

void
StatusBar::setSizeGripVisible( bool visible )
{
    ui.sizeGrip->setVisible( visible );
}

void
StatusBar::setStatus()
{
    if( m_scrobbleCount > 0 ) {
        showMessage( tr("Logged in as %1 (%2 scrobbles)").arg( lastfm::ws::Username, QString("%L1").arg( m_scrobbleCount ) ) );
    } else {
        showMessage( tr("Logged in as %1").arg( lastfm::ws::Username ));
    }
}

void
StatusBar::onGotUserInfo(lastfm::UserDetails userDetails)
{
    m_scrobbleCount = userDetails.scrobbleCount();
    setStatus();
}

void
StatusBar::onConnectionUp()
{
    const char icon[] = {0xE3, 0x80, 0x88, 0xE2, 0x8B, 0xAF, 0xE3, 0x80, 0x89, 0x00 };
    m_inetStatus->setText( QString::fromUtf8( icon ) + tr( "Online"));
}

void
StatusBar::onConnectionDown()
{
    m_inetStatus->setText( QString::fromUtf8( ":(" ) + tr( "Offline"));
}

void
StatusBar::onIPodDetected( QString iPod )
{
    showMessage( tr("iPod Detected... ") + iPod );
}

void
StatusBar::onProcessingScrobbles()
{
    showMessage( tr("Processing iPod Scrobbles...") );
}

void
StatusBar::onFoundScrobbles( QList<lastfm::Track> tracks )
{
    m_scrobbleCount += tracks.count();
    setStatus();

    tracks.count() == 1 ?
        showMessage( tr("%1 scrobble found").arg( tracks.count() ) ):
        showMessage( tr("%1 scrobbles found").arg( tracks.count() ) );

    // Schedule the status message to return to normal
    QTimer::singleShot( 10 * 1000, this, SLOT(setStatus()) );
}

void
StatusBar::onNoScrobblesFound()
{
    showMessage( tr("No scrobbles found") );

    // Schedule the status message to return to normal
    QTimer::singleShot( 10 * 1000, this, SLOT(setStatus()) );
}

void
StatusBar::onScrobblesCached( const QList<lastfm::Track>& tracks )
{
    foreach ( lastfm::Track track, tracks )
        connect( track.signalProxy(), SIGNAL(scrobbleStatusChanged()), SLOT(onScrobbleStatusChanged()));
}

void
StatusBar::onScrobbleStatusChanged()
{
    if (static_cast<lastfm::TrackData*>(sender())->scrobbleStatus == lastfm::Track::Submitted)
    {
        ++m_scrobbleCount;
        setStatus();
    }
}
