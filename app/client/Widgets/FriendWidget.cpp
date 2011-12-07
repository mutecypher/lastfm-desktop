
#include <QHBoxLayout>

#include <lastfm/RadioStation.h>

#include "lib/unicorn/widgets/AvatarWidget.h"
#include "lib/unicorn/widgets/Label.h"

#include "../Application.h"
#include "PlayableItemWidget.h"

#include "FriendWidget.h"
#include "ui_FriendWidget.h"



FriendWidget::FriendWidget( const lastfm::XmlQuery& user, QWidget* parent)
    :StylableWidget( parent ),
      ui( new Ui::FriendWidget ),
      m_user( user ),
      m_order( 0 - 1 ),
      m_listeningNow( false )
{   
    ui->setupUi( this );

    m_recentTrack.setTitle( user["recenttrack"]["name"].text() );
    m_recentTrack.setAlbum( user["recenttrack"]["album"]["name"].text() );
    m_recentTrack.setArtist( user["recenttrack"]["artist"]["name"].text() );
    m_recentTrack.setExtra( "playerName", user["scrobblesource"]["name"].text() );
    m_recentTrack.setExtra( "playerURL", user["scrobblesource"]["url"].text() );

    QString recentTrackDate = user["recenttrack"].attribute( "uts" );

    m_listeningNow = recentTrackDate.isEmpty();

    if ( !m_listeningNow )
        m_recentTrack.setTimeStamp( QDateTime::fromTime_t( recentTrackDate.toUInt() ) );

    QRegExp re( "/serve/(\\d*)s?/" );
    ui->avatar->loadUrl( user["image size=medium"].text().replace( re, "/serve/\\1s/" ), HttpImageWidget::ScaleNone );
    ui->avatar->setHref( user["url"].text() );

    setDetails();

    ui->radio->setStation( RadioStation::library( User( user["name"].text() ) ), tr( "%2%1s Library Radio" ).arg( QChar( 0x2019 ), user["name"].text() ) );

    ui->avatar->setUser( m_user );
}


void
FriendWidget::update( const lastfm::XmlQuery& user, unsigned int order )
{
    m_order = order;

    m_recentTrack.setTitle( user["recenttrack"]["name"].text() );
    m_recentTrack.setAlbum( user["recenttrack"]["album"]["name"].text() );
    m_recentTrack.setArtist( user["recenttrack"]["artist"]["name"].text() );
    m_recentTrack.setExtra( "playerName", user["scrobblesource"]["name"].text() );
    m_recentTrack.setExtra( "playerURL", user["scrobblesource"]["url"].text() );

    QDateTime timestamp = QDateTime::fromString( user["recenttrack"].attribute( "date" ), "d MMM yyyy, hh:mm" );
    timestamp.setTimeSpec( Qt::UTC );
    m_recentTrack.setTimeStamp( timestamp.toLocalTime() );

    setDetails();
}

void
FriendWidget::setOrder( int order )
{
    m_order = order;
}

void
FriendWidget::setListeningNow( bool listeningNow )
{
    m_listeningNow = listeningNow;
}

bool
FriendWidget::operator<( const FriendWidget& that ) const
{
    // sort by most recently listened and then by name

    if ( this->m_listeningNow && !that.m_listeningNow )
        return true;

    if ( !this->m_listeningNow && that.m_listeningNow )
        return false;

    if ( this->m_listeningNow && that.m_listeningNow )
        return this->name().toLower() < that.name().toLower();

    if ( !this->m_recentTrack.timestamp().isNull() && that.m_recentTrack.timestamp().isNull() )
        return true;

    if ( this->m_recentTrack.timestamp().isNull() && !that.m_recentTrack.timestamp().isNull() )
        return false;

    if ( this->m_recentTrack.timestamp().isNull() && that.m_recentTrack.timestamp().isNull() )
        return this->name().toLower() < that.name().toLower();

    // both timestamps are valid!

    if ( this->m_recentTrack.timestamp() == that.m_recentTrack.timestamp() )
    {
        if ( this->m_order == that.m_order )
            return this->name().toLower() < that.name().toLower();

        return this->m_order < that.m_order;
    }

    // this is the other way around because a higher time means it's lower in the list
    return this->m_recentTrack.timestamp() > that.m_recentTrack.timestamp();
}


void
FriendWidget::setDetails()
{
    QString nameString = name();
    QString realnameString = realname();

    if ( !realnameString.isEmpty() )
        nameString += QString( " - %1" ).arg( realnameString );

    QString player = Label::anchor( m_recentTrack.extra( "playerURL" ), m_recentTrack.extra( "playerName" ) );
    QString track = Label::anchor( m_recentTrack.artist().www().toString(), m_recentTrack.artist().name() );
    track.append( QString( " " ) + QChar(8211) + " " );
    track.append( Label::anchor( m_recentTrack.www().toString(), m_recentTrack.title() ) );

    ui->name->setText( nameString );
    ui->lastTrack->setText( tr( "%1 on %2" ).arg( track, player ) );

    if ( m_listeningNow )
        ui->timestamp->setText( tr( "Listening now" ) );
    else
        ui->timestamp->setText( unicorn::Label::prettyTime( m_recentTrack.timestamp() )  );
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
