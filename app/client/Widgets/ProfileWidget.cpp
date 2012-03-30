
#include <QBoxLayout>
#include <QLabel>

#include <lastfm/Library.h>
#include <lastfm/XmlQuery.h>

#include "lib/unicorn/widgets/AvatarWidget.h"
#include "lib/unicorn/StylableWidget.h"

#include "PlayableItemWidget.h"
#include "ProfileArtistWidget.h"
#include "ContextLabel.h"

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
    ui.avatar->loadUrl( userDetails.imageUrl( lastfm::Large, true ), HttpImageWidget::ScaleNone );
    ui.avatar->setHref( userDetails.www() );

    connect( lastfm::Library::getArtists( userDetails.name(), 1 ), SIGNAL(finished()), SLOT(onGotLibraryArtists()));

    setScrobbleCount();
}

void
ProfileWidget::onGotLibraryArtists()
{
    lastfm::XmlQuery lfm;

    if ( lfm.parse( static_cast<QNetworkReply*>(sender())->readAll() ) )
    {
        int scrobblesPerDay = aApp->currentSession()->userInfo().scrobbleCount() / aApp->currentSession()->userInfo().dateRegistered().daysTo( QDateTime::currentDateTime() );
        int totalArtists = lfm["artists"].attribute( "total" ).toInt();

        ui.context->setText( tr( "You have %L1 artists in you library and on average listen to %L2 tracks per day." ).arg( totalArtists ).arg( scrobblesPerDay ) );
        ui.context->show();
    }
    else
    {
        qDebug() << lfm.parseError().message() << lfm.parseError().enumValue();
    }
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

        vl->addWidget( ui.scrobbleCount = new QLabel( "0" ) );
        ui.scrobbleCount->setObjectName( "scrobbleCount" );

        vl->addWidget( ui.scrobbles = new QLabel( tr( "Scrobbles" ) ) );
        ui.scrobbleCount->setObjectName( "scrobbles" );

        vl->addWidget( ui.lovedCount = new QLabel( "0" ) );
        ui.lovedCount->setObjectName( "lovedCount" );

        vl->addWidget( ui.loved = new QLabel( tr( "Loved tracks" ) ) );
        ui.loved->setObjectName( "loved" );

        layout->addWidget( ui.context = new ContextLabel( this ) );
        ui.context->setObjectName( "userBlurb" );
        ui.context->setTextFormat( Qt::RichText );
        ui.context->setWordWrap( true );
        ui.context->hide();

        {
            QFrame* splitter = new QFrame( this );
            layout->addWidget( splitter );
            splitter->setObjectName( "splitter" );

            QLabel* title = new QLabel( tr("Top Artists This Week"), this ) ;
            layout->addWidget( title );
            title->setObjectName( "title" );
            layout->addWidget( ui.topWeeklyArtists = new StylableWidget( this ) );
            ui.topWeeklyArtists->setObjectName( "section" );
        }

        {
            QFrame* splitter = new QFrame( this );
            layout->addWidget( splitter );
            splitter->setObjectName( "splitter" );

            QLabel* title = new QLabel( tr("Top Artists Overall"), this ) ;
            layout->addWidget( title );
            title->setObjectName( "title" );
            layout->addWidget( ui.topOverallArtists = new StylableWidget( this ) );
            ui.topOverallArtists->setObjectName( "section" );
        }

        layout->addStretch();

        lastfm::User user = lastfm::User( newUsername );

        connect( user.getLovedTracks( 1 ), SIGNAL(finished()), SLOT(onGotLovedTracks()) );
        connect( user.getTopArtists( "overall", 5, 1 ), SIGNAL(finished()), SLOT(onGotTopOverallArtists()));
        connect( user.getTopArtists( "7day", 5, 1 ), SIGNAL(finished()), SLOT(onGotTopWeeklyArtists()));
    }
}


void
ProfileWidget::onGotTopWeeklyArtists()
{
    lastfm::XmlQuery lfm;

    if ( lfm.parse( qobject_cast<QNetworkReply*>(sender())->readAll() ) )
    {
        QVBoxLayout* layout = new QVBoxLayout( ui.topWeeklyArtists );
        layout->setContentsMargins( 0, 0, 0, 0 );
        layout->setSpacing( 0 );

        int maxPlays = lfm["topartists"]["artist"]["playcount"].text().toInt();

        foreach ( const lastfm::XmlQuery& artist, lfm["topartists"].children("artist") )
            layout->addWidget( new ProfileArtistWidget( artist, maxPlays, this ) );
    }
    else
    {
        qDebug() << lfm.parseError().message() << lfm.parseError().enumValue();
    }
}


void
ProfileWidget::onGotTopOverallArtists()
{
    lastfm::XmlQuery lfm;

    if ( lfm.parse( qobject_cast<QNetworkReply*>(sender())->readAll() ) )
    {
        QVBoxLayout* layout = new QVBoxLayout( ui.topOverallArtists );
        layout->setContentsMargins( 0, 0, 0, 0 );
        layout->setSpacing( 0 );

        int maxPlays = lfm["topartists"]["artist"]["playcount"].text().toInt();

        foreach ( const lastfm::XmlQuery& artist, lfm["topartists"].children("artist") )
            layout->addWidget( new ProfileArtistWidget( artist, maxPlays, this ) );
    }
    else
    {
        qDebug() << lfm.parseError().message() << lfm.parseError().enumValue();
    }
}

void
ProfileWidget::onGotLovedTracks()
{
    lastfm::XmlQuery lfm;

    if ( lfm.parse( qobject_cast<QNetworkReply*>(sender())->readAll() ) )
    {
        ui.lovedCount->setText( tr( "%L1" ).arg( lfm["lovedtracks"].attribute( "total" ).toInt() ) );
    }
    else
    {
        qDebug() << lfm.parseError().message() << lfm.parseError().enumValue();
    }
}


void
ProfileWidget::onScrobblesCached( const QList<lastfm::Track>& tracks )
{
    foreach ( lastfm::Track track, tracks )
        connect( track.signalProxy(), SIGNAL(scrobbleStatusChanged( short )), SLOT(onScrobbleStatusChanged( short )));
}

void
ProfileWidget::onScrobbleStatusChanged( short scrobbleStatus )
{
    if (scrobbleStatus == lastfm::Track::Submitted)
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
