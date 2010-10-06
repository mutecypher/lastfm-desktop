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
#include "ScrobbleControls.h"

using unicorn::Session;
ProfileWidget::ProfileWidget( QWidget* p )
           :StylableWidget( p )
{
    QHBoxLayout* mainLayout = new QHBoxLayout( this);

    QWidget* profileBox = new StylableWidget();
    {
        QHBoxLayout* profileLayout = new QHBoxLayout( profileBox );

        profileLayout->addWidget( ui.avatar = new HttpImageWidget());
        ui.avatar->setObjectName( "avatar" );
        ui.avatar->setToolTip( tr( "Visit Last.fm profile" ) );
        ui.avatar->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

        QVBoxLayout* userDetails = new QVBoxLayout();

        userDetails->addWidget( ui.welcomeLabel = new QLabel(), 0, Qt::AlignTop );
        ui.welcomeLabel->setObjectName( "title" );

        userDetails->addWidget( ui.scrobbleMeter = new ScrobbleMeter() );
        userDetails->addWidget( ui.since = new QLabel() );

        profileLayout->addLayout( userDetails );
    }

    mainLayout->addWidget( profileBox );

    QWidget* titleBox = new QWidget();
    {
        QVBoxLayout* vl = new QVBoxLayout( titleBox );
        vl->addWidget( ui.title1 = new QLabel());
        ui.title1->setObjectName( "title1" );
        ui.title1->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
        ui.title1->setOpenExternalLinks( true );

        vl->addWidget( ui.title2 = new QLabel());
        ui.title2->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
        ui.title2->setObjectName( "title2" );
        ui.title2->setOpenExternalLinks( true );

        vl->addStretch( 1 );

        vl->addWidget( ui.sc = new ScrobbleControls());
        vl->addStretch();
    }

    mainLayout->addWidget( titleBox );
    
    //On first run we won't catch the sessionChanged signal on time
    //so we should try to get the current session from the unicorn::Application 
    unicorn::Session* currentSession = qobject_cast<unicorn::Application*>( qApp )->currentSession();
    if ( currentSession )
    {
        onSessionChanged( currentSession );
    }
    connect( qApp, SIGNAL( sessionChanged( unicorn::Session* ) ), SLOT( onSessionChanged( unicorn::Session* ) ) );
    connect( qApp, SIGNAL(scrobblesCached(QList<lastfm::Track>)), SLOT(onScrobblesCached(QList<lastfm::Track>)));

    connect( qApp, SIGNAL( trackStarted(Track, Track) ), SLOT( onTrackStarted(Track, Track) ) );
    //connect( qApp, SIGNAL( paused() ), SLOT( onPaused() ) );
    //connect( qApp, SIGNAL( resumed() ), SLOT( onResumed() ) );
    connect( qApp, SIGNAL( stopped() ), SLOT( onStopped() ) );
}

void
ProfileWidget::onTrackStarted( const Track& t, const Track& previous )
{
    const unsigned short em_dash = 0x2014;
    QString title = QString("<a class='title' href=\"%1\">%2</a> ") + QChar(em_dash) + " <a class='title' href=\"%3\">%4</a>";
    const unicorn::Application* uApp = qobject_cast<unicorn::Application*>(qApp);

    ui.title1->setText( "<style>" + uApp->loadedStyleSheet() + "</style>" + title.arg(t.artist().www().toString(), t.artist(), t.www().toString(), t.title()));
    if( !t.album().isNull() )
    {
        QString album("from <a class='title' href=\"%1\">%2</a>");
        ui.title2->setText("<style>" + uApp->loadedStyleSheet() + "</style>" + album.arg( t.album().www().toString(), t.album().title()));
    }
    else
    {
        ui.title2->clear();
    }
}

void
ProfileWidget::onStopped()
{
    ui.title1->clear();
    ui.title2->clear();
}

void 
ProfileWidget::onSessionChanged( Session* session )
{
    if ( !session )
        return;

    qDebug() << "profile widget: session change";
    ui.welcomeLabel->setText( tr( "%1's Profile" ).arg( session->userInfo().name() ) );
    ui.since->clear(); 
    ui.scrobbleMeter->clear();
    ui.avatar->clear();

    updateUserInfo( session->userInfo() );
    connect( session, SIGNAL( userInfoUpdated( const lastfm::UserDetails& ) ),
             this, SLOT( updateUserInfo( const lastfm::UserDetails& ) ) );
}

void 
ProfileWidget::updateUserInfo( const lastfm::UserDetails& userdetails )
{
    qDebug() << "user info updated";
    ui.scrobbleMeter->setCount( userdetails.scrobbleCount() );
    int const daysRegistered = userdetails.dateRegistered().daysTo( QDateTime::currentDateTime());
    int const weeksRegistered = daysRegistered / 7;
    QString sinceText = tr("Scrobbles since %1" ).arg( userdetails.dateRegistered().toString( "d MMM yyyy"));

    if( weeksRegistered )
        sinceText += "\n(" + tr( "That's about %1 tracks a week" ).arg( userdetails.scrobbleCount() / weeksRegistered ) + ")";
    else
        sinceText = "";

    ui.since->setText( sinceText );
    ui.avatar->loadUrl( userdetails.imageUrl( lastfm::Medium ), false );
    ui.avatar->setHref( userdetails.www());
}

void
ProfileWidget::onScrobblesCached( const QList<lastfm::Track>& tracks )
{
    foreach ( lastfm::Track track, tracks )
        connect( track.signalProxy(), SIGNAL(scrobbleStatusChanged()), SLOT(onScrobbleStatusChanged()));
}

void
ProfileWidget::onScrobbleStatusChanged()
{
    if (static_cast<lastfm::TrackData*>(sender())->scrobbleStatus == lastfm::Track::Submitted)
        *ui.scrobbleMeter += 1;
}

