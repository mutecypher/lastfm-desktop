
#include <QHBoxLayout>

#include <lastfm/RadioStation.h>
#include <lastfm/User.h>
#include <lastfm/Track.h>
#include <lastfm/XmlQuery.h>

#include "lib/unicorn/widgets/AvatarWidget.h"
#include "lib/unicorn/widgets/Label.h"

#include "../Application.h"
#include "PlayableItemWidget.h"

#include "FriendWidget.h"



FriendWidget::FriendWidget( const lastfm::XmlQuery& user, QWidget* parent)
    :StylableWidget( parent ), m_user( user )
{   
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    layout->addWidget( ui.avatar = new AvatarWidget( this ) );
    ui.avatar->setObjectName( "avatar" );

    QRegExp re( "/serve/(\\d*)s?/" );

    ui.avatar->loadUrl( user["image size=medium"].text().replace( re, "/serve/\\1s/" ), HttpImageWidget::ScaleNone );
    ui.avatar->setHref( user["url"].text() );

    QVBoxLayout* vl = new QVBoxLayout();
    vl->setContentsMargins( 0, 0, 0, 0 );
    vl->setSpacing( 0 );
    layout->addLayout( vl, 1 );

    vl->addWidget( ui.name = new QLabel( "", this) );
    ui.name->setObjectName( "name" );
    vl->addWidget( ui.lastTrack = new QLabel( "", this) );
    ui.lastTrack->setObjectName( "lastTrack" );
    setDetails();

    vl->addWidget( ui.radio = new PlayableItemWidget( RadioStation::library( User( user["name"].text() ) ), tr( "%2%1s Library Radio" ).arg( QChar( 0x2019 ), user["name"].text() ) ) );
    ui.radio->setObjectName( "radio" );

    // Don't get user details for each friend, it is silly and wasteful
    //connect( lastfm::UserDetails::getInfo( user["name"].text() ), SIGNAL(finished()), SLOT(onGotInfo()));
}

void
FriendWidget::onGotInfo()
{
    lastfm::UserDetails user( qobject_cast<QNetworkReply*>(sender()) );

    ui.avatar->setUserDetails( user );
    m_userDetails = user;
    setDetails();
}


void
FriendWidget::setDetails()
{
    lastfm::MutableTrack recentTrack;
    recentTrack.setTitle( m_user["recenttrack"]["name"].text() );
    recentTrack.setAlbum( m_user["recenttrack"]["album"]["name"].text() );
    recentTrack.setArtist( m_user["recenttrack"]["artist"]["name"].text() );

    QString nameString = name();
    QString realnameString = realname();

    if ( !realnameString.isEmpty() )
        nameString += QString( " - %1" ).arg( realnameString );

    ui.name->setText( nameString );
    ui.lastTrack->setText( tr( "Last track: %1" ).arg( recentTrack.toString() ) );

}

QString
FriendWidget::name() const
{
    return m_user["name"].text();
}

QString
FriendWidget::realname() const
{
    return m_user["realname"].text();
}
