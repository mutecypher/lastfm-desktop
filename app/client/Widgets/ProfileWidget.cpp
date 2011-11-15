
#include <QBoxLayout>
#include <QLabel>

#include "lib/unicorn/widgets/AvatarWidget.h"
#include "lib/unicorn/StylableWidget.h"

#include "PlayableItemWidget.h"
#include "ProfileArtistWidget.h"

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

        QGridLayout* grid = new QGridLayout;

        vl->addLayout( grid );

        grid->addWidget( ui.scrobbleCount = new QLabel( "0" ), 0, 0 );
        ui.scrobbleCount->setObjectName( "scrobbleCount" );

        grid->addWidget( ui.scrobbles = new QLabel( tr( "Scrobbles" ) ), 1, 0 );
        ui.scrobbleCount->setObjectName( "scrobbles" );

        QFrame* splitter = new QFrame;
        splitter->setObjectName( "splitter" );
        splitter->setFrameStyle( QFrame::VLine );
        grid->addWidget( splitter, 0, 1, 2, 1, Qt::AlignLeft );

        grid->addWidget( ui.lovedCount = new QLabel( "0" ), 0, 2 );
        ui.lovedCount->setObjectName( "lovedCount" );

        grid->addWidget( ui.loved = new QLabel( tr( "Loved tracks" ) ), 1, 2 );
        ui.loved->setObjectName( "loved" );

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

        layout->addStretch( 1 );

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
