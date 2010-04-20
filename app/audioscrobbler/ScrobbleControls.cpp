/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Jono Cole and Doug Mansell

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

#include "ScrobbleControls.h"
#include "Application.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QAction>
#include <lastfm/User>
#include "lib/unicorn/widgets/UserMenu.h"

ScrobbleControls::ScrobbleControls()
{
    new QHBoxLayout( this );
    layout()->setContentsMargins( 0, 0, 0, 0 );

    QToolButton* tb = new QToolButton();
    tb->setIconSize( QSize( 32, 32 ));
    tb->setCheckable( true );

    if( unicorn::Settings().userRoster().count() > 1 ) {
        tb->setMenu( new UserMenu());
        tb->setPopupMode( QToolButton::MenuButtonPopup );
    }

    connect( tb, SIGNAL( toggled( bool )), window(), SLOT( toggleProfile( bool )));
    connect( qApp, SIGNAL( sessionChanged( unicorn::Session, unicorn::Session )), SLOT( onSessionChanged()));
    connect( qApp, SIGNAL( gotUserInfo( lastfm::UserDetails )), SLOT( onUserGotInfo( lastfm::UserDetails )));
    connect( qApp, SIGNAL( rosterUpdated()), SLOT( onRosterUpdated()));
    layout()->addWidget(tb);
    
    layout()->addWidget(ui.love = new QPushButton(tr("love")));
    ui.love->setObjectName("love");
    
    layout()->addWidget(ui.tag = new QPushButton(tr("tag")));
    ui.tag->setObjectName("tag");
    
    layout()->addWidget(ui.share = new QPushButton(tr("share")));
    ui.share->setObjectName("share");

}

void 
ScrobbleControls::toggleProfile( bool profile ) {
    if( profile ) {
        QTimer::singleShot( 0, window(), SLOT( showNowScrobbling()));
    } else {
        QTimer::singleShot( 0, window(), SLOT( showProfile() ));
    }
}

void
ScrobbleControls::setEnabled( bool enabled )
{
    ui.love->setEnabled( enabled );
    ui.tag->setEnabled( enabled );
    ui.share->setEnabled( enabled );
}

void
ScrobbleControls::setLoveAction( const QAction* a )
{
    connect( ui.love, SIGNAL(clicked()), a, SLOT(trigger()));
}

void
ScrobbleControls::setTagAction( const QAction* a )
{
    connect( ui.tag, SIGNAL(clicked()), a, SLOT(trigger()));
}

void
ScrobbleControls::setShareAction( const QAction* a )
{
    connect( ui.share, SIGNAL(clicked()), a, SLOT(trigger()));
}

void 
ScrobbleControls::onSessionChanged()
{
    findChild<QToolButton*>()->setIcon( QIcon());
}

void 
ScrobbleControls::onUserGotInfo( const lastfm::UserDetails& user )
{
    connect( lastfm::nam()->get(QNetworkRequest( user.mediumImageUrl())), SIGNAL( finished()),
                                                                          SLOT( onImageDownloaded()));
}

void 
ScrobbleControls::onImageDownloaded()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>( sender());
    Q_ASSERT( reply);

    QImage img = QImage::fromData( reply->readAll());    
    findChild<QToolButton*>()->setIcon( QIcon(QPixmap::fromImage( img)));
}

void 
ScrobbleControls::onRosterUpdated()
{
    QToolButton* tb = findChild<QToolButton*>();
    if( unicorn::Settings().userRoster().count() > 1 ) {
        tb->setMenu( new UserMenu());
        tb->setPopupMode( QToolButton::MenuButtonPopup );
        tb->setStyle( QApplication::style());
    } else {
        tb->setPopupMode( QToolButton::DelayedPopup );
        tb->menu()->deleteLater();
        tb->setStyle( QApplication::style());
    }
}
