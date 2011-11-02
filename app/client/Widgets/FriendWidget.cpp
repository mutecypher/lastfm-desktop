
#include <QHBoxLayout>

#include <lastfm/RadioStation.h>

#include "lib/unicorn/widgets/AvatarWidget.h"
#include "lib/unicorn/widgets/Label.h"

#include "../Application.h"
#include "PlayableItemWidget.h"

#include "FriendWidget.h"



FriendWidget::FriendWidget( const lastfm::XmlQuery& user, QWidget* parent)
    :StylableWidget( parent ), m_user( user )
{   
    m_recentTrack.setTitle( user["recenttrack"]["name"].text() );
    m_recentTrack.setAlbum( user["recenttrack"]["album"]["name"].text() );
    m_recentTrack.setArtist( user["recenttrack"]["artist"]["name"].text() );

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

    vl->addWidget( ui.name = new Label( "", this) );
    ui.name->setObjectName( "name" );
    vl->addWidget( ui.lastTrack = new Label( "", this) );
    ui.lastTrack->setObjectName( "lastTrack" );
    setDetails();

    vl->addWidget( ui.radio = new PlayableItemWidget( RadioStation::library( User( user["name"].text() ) ), tr( "%2%1s Library Radio" ).arg( QChar( 0x2019 ), user["name"].text() ) ) );
    ui.radio->setObjectName( "radio" );

    ui.avatar->setUser( m_user );
}


void
FriendWidget::setDetails()
{
    QString nameString = name();
    QString realnameString = realname();

    if ( !realnameString.isEmpty() )
        nameString += QString( " - %1" ).arg( realnameString );

    ui.name->setText( nameString );
    ui.lastTrack->setText( tr( "Last track: %1" ).arg( m_recentTrack.toString() ) );

}

QString
FriendWidget::name() const
{
    return m_user.name();
}

QString
FriendWidget::realname() const
{
    return m_user.realName();
}
