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
#include <QPushButton>
#include <QDesktopServices>

#include <lastfm/ws.h>
#include <lastfm/User>
#include <lastfm/Audioscrobbler>
#include <lastfm/Track>

#include "lib/unicorn/UnicornApplication.h"
#include "lib/unicorn/UnicornSession.h"
#include "lib/unicorn/widgets/HttpImageWidget.h"
#include "lib/unicorn/widgets/LfmListViewWidget.h"
#include "lib/unicorn/widgets/DataBox.h"

#include "ProfileWidget.h"
#include "ScrobbleMeter.h"
#include "RecentTracksWidget.h"

using unicorn::Session;
ProfileWidget::ProfileWidget( QWidget* p )
           :StylableWidget( p )
{
    QVBoxLayout* l = new QVBoxLayout( this );
    QHBoxLayout* userDetails = new QHBoxLayout();
    userDetails->addWidget( ui.avatar = new HttpImageWidget());
    ui.avatar->setObjectName( "avatar" );
    ui.avatar->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    userDetails->addWidget( ui.welcomeLabel = new QLabel(), 0, Qt::AlignTop );
    ui.welcomeLabel->setObjectName( "title" );

    l->addLayout( userDetails );
    QFrame* scrobbleDetails = new QFrame();
    scrobbleDetails->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );
    scrobbleDetails->setObjectName( "ScrobbleDetails" );

    new QVBoxLayout( scrobbleDetails );
    qobject_cast<QVBoxLayout*>(scrobbleDetails->layout())->addWidget( ui.scrobbleMeter = new ScrobbleMeter(), 0, Qt::AlignHCenter );
    scrobbleDetails->layout()->addWidget( ui.since = new QLabel()); 
    ui.since->setAlignment( Qt::AlignCenter );

    ui.recentTracks = new RecentTracksWidget( lastfm::ws::Username, this );
    ui.recentTracks->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::MinimumExpanding );

    DataBox* recentTrackBox = new DataBox( tr( "Recently scrobbled tracks" ), ui.recentTracks );
    recentTrackBox->setObjectName( "recentTracks" );

    l->addWidget( scrobbleDetails );
    l->addWidget( recentTrackBox );
    
    connect( qApp, SIGNAL(sessionChanged(unicorn::Session, unicorn::Session)), SLOT(onSessionChanged(unicorn::Session)));
    connect( qApp, SIGNAL(gotUserInfo(lastfm::UserDetails)), SLOT(onGotUserInfo(lastfm::UserDetails)));
    connect( qApp, SIGNAL(scrobblesCached(QList<lastfm::Track>)), SLOT(onScrobblesCached(QList<lastfm::Track>)));
    connect( qApp, SIGNAL(scrobblesSubmitted(QList<lastfm::Track>, int)), SLOT(onScrobblesSubmitted(QList<lastfm::Track>, int)));
}

void 
ProfileWidget::onSessionChanged( const Session& session )
{
    ui.welcomeLabel->setText( tr("%1's Profile" ).arg( session.username() ));
    ui.since->clear(); 
    ui.scrobbleMeter->clear();
    ui.avatar->clear();

    ui.recentTracks->setUsername( session.username() );
}

void 
ProfileWidget::onGotUserInfo( const lastfm::UserDetails& userdetails )
{
    ui.scrobbleMeter->setCount( userdetails.scrobbleCount() );
    int const daysRegistered = userdetails.dateRegistered().daysTo( QDateTime::currentDateTime());
    int const weeksRegistered = daysRegistered / 7;
    QString sinceText = tr("Scrobbles since %1" ).arg( userdetails.dateRegistered().toString( "d MMM yyyy"));
    sinceText += "\n(" + tr( "That's about %1 tracks a week" ).arg( userdetails.scrobbleCount() / weeksRegistered ) + ")";
    ui.since->setText( sinceText );
    ui.avatar->loadUrl( userdetails.imageUrl( lastfm::Medium ));
    ui.avatar->setHref( userdetails.www());
}

void
ProfileWidget::onScrobblesCached( const QList<lastfm::Track>& tracks )
{
    foreach ( lastfm::Track track, tracks )
        ui.recentTracks->addCachedTrack( track );

    ui.recentTracks->write();
}

void 
ProfileWidget::onScrobblesSubmitted( const QList<lastfm::Track>& tracks, int succeeded )
{
    *ui.scrobbleMeter += succeeded;

    foreach ( lastfm::Track track, tracks )
        ui.recentTracks->addScrobbledTrack( track );

    ui.recentTracks->write();
}
