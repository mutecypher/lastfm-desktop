
#include <QBoxLayout>
#include <QLabel>

#include "lib/unicorn/widgets/AvatarWidget.h"
#include "lib/unicorn/StylableWidget.h"

#include "PlayableItemWidget.h"

#include "../Services/ScrobbleService/ScrobbleService.h"
#include "../Application.h"

#include "ProfileWidget.h"

ProfileWidget::ProfileWidget(QWidget *parent)
    :StylableWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    connect( aApp, SIGNAL(sessionChanged(unicorn::Session*)), SLOT(onSessionChanged(unicorn::Session*)) );
    connect( aApp, SIGNAL(gotUserInfo(lastfm::User)), SLOT(onGotUserInfo(lastfm::User)) );

    connect( &ScrobbleService::instance(), SIGNAL(scrobblesCached(QList<lastfm::Track>)), SLOT(onScrobblesCached(QList<lastfm::Track>)));

    onSessionChanged( aApp->currentSession() );
}

void
ProfileWidget::onSessionChanged( unicorn::Session* session )
{  
    onGotUserInfo( session->userInfo() );
}


void
ProfileWidget::onGotUserInfo( const lastfm::User& userDetails )
{
    changeUser( userDetails.name() );

    m_scrobbleCount = userDetails.scrobbleCount();
    ui.avatar->setUser( userDetails );
    ui.avatar->loadUrl( userDetails.imageUrl( lastfm::Medium, true ), HttpImageWidget::ScaleNone );
    ui.avatar->setHref( userDetails.www() );

    setScrobbleCount();
}


void
ProfileWidget::changeUser( const QString& newUsername )
{
    if ( !newUsername.isEmpty() && newUsername != m_currentUsername )
    {
        m_currentUsername = newUsername;

        // Make sure we don't recieve any updates about the last session
        disconnect( this, SLOT(onGotTopOverallArtists()) );
        disconnect( this, SLOT(onGotTopWeeklyArtists()) );

        layout()->removeWidget( m_main );
        delete m_main;

        layout()->addWidget( m_main = new QWidget( this ) );
        QVBoxLayout* layout = new QVBoxLayout( m_main );
        layout->setContentsMargins( 0, 0, 0, 0 );
        layout->setSpacing( 0 );

        layout->addWidget( ui.user = new StylableWidget( this ) );
        ui.user->setObjectName( "user" );
        QHBoxLayout* hl = new QHBoxLayout( ui.user );
        hl->setContentsMargins( 0, 0, 0, 0 );
        hl->setSpacing( 0 );

        hl->addWidget( ui.avatar = new AvatarWidget( this ) );
        ui.avatar->setObjectName( "avatar" );

        QVBoxLayout* vl = new QVBoxLayout();
        vl->setContentsMargins( 0, 0, 0, 0 );
        vl->setSpacing( 0 );

        hl->addLayout( vl, 1 );

        vl->addWidget( ui.name = new QLabel( newUsername, this) );
        ui.name->setObjectName( "name" );

        vl->addWidget( ui.scrobbleCount = new QLabel( "", this ) );
        ui.scrobbleCount->setObjectName( "scrobbleCount" );

        vl->addWidget( ui.scrobbles = new QLabel( tr("Scrobbles"), this) );
        ui.scrobbleCount->setObjectName( "scrobbles" );

        {
            QLabel* title = new QLabel( tr("Top Weekly Artists"), this ) ;
            layout->addWidget( title );
            title->setObjectName( "title" );
            layout->addWidget( ui.topWeeklyArtists = new StylableWidget( this ) );
            ui.topWeeklyArtists->setObjectName( "section" );
        }

        {
            QLabel* title = new QLabel( tr("Top Overall Artists"), this ) ;
            layout->addWidget( title );
            title->setObjectName( "title" );
            layout->addWidget( ui.topOverallArtists = new StylableWidget( this ) );
            ui.topOverallArtists->setObjectName( "section" );
        }

        layout->addStretch( 1 );

        connect( User( newUsername ).getTopArtists( "overall", 5, 1 ), SIGNAL(finished()), SLOT(onGotTopOverallArtists()));
        connect( User( newUsername ).getTopArtists( "7day", 5, 1 ), SIGNAL(finished()), SLOT(onGotTopWeeklyArtists()));
    }
}


void
ProfileWidget::onGotTopWeeklyArtists()
{
    try
    {
        QVBoxLayout* layout = new QVBoxLayout( ui.topWeeklyArtists );
        layout->setContentsMargins( 0, 0, 0, 0 );
        layout->setSpacing( 0 );

        lastfm::XmlQuery lfm;
        lfm.parse( qobject_cast<QNetworkReply*>(sender())->readAll() );

        foreach ( const lastfm::XmlQuery& artist, lfm["topartists"].children("artist") )
        {
            QString artistName = artist["name"].text();
            int playCount = artist["playcount"].text().toInt();

            PlayableItemWidget* item = new PlayableItemWidget( RadioStation::similar( lastfm::Artist( artistName ) ), tr( "%1 Similar Radio" ).arg( artistName ), tr( "(%L1 plays)" ).arg( playCount ) );
            item->setObjectName( "station" );
            layout->addWidget( item );
        }
    }
    catch ( lastfm::ws::ParseError error )
    {
        qDebug() << error.message();
    }
}


void
ProfileWidget::onGotTopOverallArtists()
{
    try
    {
        QVBoxLayout* layout = new QVBoxLayout( ui.topOverallArtists );
        layout->setContentsMargins( 0, 0, 0, 0 );
        layout->setSpacing( 0 );

        lastfm::XmlQuery lfm;
        lfm.parse( qobject_cast<QNetworkReply*>(sender())->readAll() );

        foreach ( const lastfm::XmlQuery& artist, lfm["topartists"].children("artist") )
        {
            QString artistName = artist["name"].text();
            int playCount = artist["playcount"].text().toInt();

            PlayableItemWidget* item = new PlayableItemWidget( RadioStation::similar( lastfm::Artist( artistName ) ), tr( "%1 Similar Radio" ).arg( artistName ), tr( "(%L1 plays)" ).arg( playCount ) );
            item->setObjectName( "station" );
            layout->addWidget( item );
        }
    }
    catch ( lastfm::ws::ParseError error )
    {
        qDebug() << error.message();
    }
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
    {
        ++m_scrobbleCount;
        setScrobbleCount();
    }
}

void
ProfileWidget::setScrobbleCount()
{
    ui.scrobbleCount->setText( QString( "%L1" ).arg( m_scrobbleCount ) );
}
