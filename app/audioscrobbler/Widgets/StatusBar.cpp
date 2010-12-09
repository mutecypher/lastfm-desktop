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

#include "../MediaDevices/DeviceScrobbler.h"
#include "../Application.h"

StatusBar::StatusBar( QWidget* parent )
    :StylableWidget( parent )
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    layout->addWidget( m_mainStatus = new QLabel( "", this ) );
    setStatus();
    layout->addStretch( 1 );
    layout->addWidget( m_inetStatus = new QLabel( tr(""), this ) );

    connect( aApp, SIGNAL( internetConnectionDown() ), SLOT( onConnectionDown() ) );
    connect( aApp, SIGNAL( internetConnectionUp() ), SLOT( onConnectionUp() ) );

    DeviceScrobbler* deviceScrobbler = aApp->deviceScrobbler();
    connect( deviceScrobbler, SIGNAL( detectedIPod( QString )), SLOT( onIPodDetected()));
    connect( deviceScrobbler, SIGNAL( foundScrobbles( QList<Track> )), SLOT( onFoundScrobbles( QList<Track> )));
    connect( deviceScrobbler, SIGNAL( noScrobblesFound()),SLOT( onNoScrobblesFound()));

    layout->addWidget( new QSizeGrip( this ), 0 , Qt::AlignBottom | Qt::AlignRight );
}

void
StatusBar::setStatus()
{
    m_mainStatus->setText( tr("Logged in as %1 (%2 scrobbles)").arg( "eartle", QString("%L1").arg( 200000 ) ) );
}

void
StatusBar::onConnectionUp()
{
    m_inetStatus->setText( tr("Online") );
}

void
StatusBar::onConnectionDown()
{
    m_inetStatus->setText( tr("Offline") );
}

void
StatusBar::onIPodDetected()
{
    m_mainStatus->setText( tr("iPod Detected...") );
}

void
StatusBar::onFoundScrobbles( QList<Track> tracks )
{
    m_mainStatus->setText( tr("%1 scrobbles found").arg( tracks.count() ) );
    QTimer::singleShot( 10 * 1000, this, SLOT(setStatus()) );
}

void
StatusBar::onNoScrobblesFound()
{
    m_mainStatus->setText( tr("No scrobbles found") );
    QTimer::singleShot( 10 * 1000, this, SLOT(setStatus()) );
}
