
#include <QBoxLayout>
#include <QLabel>

#include <lastfm/Library.h>
#include <lastfm/XmlQuery.h>

#include "lib/unicorn/widgets/Label.h"
#include "lib/unicorn/widgets/AvatarWidget.h"

#include "PlayableItemWidget.h"
#include "ProfileArtistWidget.h"
#include "ContextLabel.h"

#include "../Services/ScrobbleService/ScrobbleService.h"
#include "../Application.h"

#include "ProfileWidget.h"

#include "ui_ProfileWidget.h"

ProfileWidget::ProfileWidget(QWidget *parent)
    :QFrame(parent), ui( new Ui::ProfileWidget )
{
    ui->setupUi( this );

    connect( aApp, SIGNAL(sessionChanged(unicorn::Session*)), SLOT(onSessionChanged(unicorn::Session*)) );
    connect( aApp, SIGNAL(gotUserInfo(lastfm::User)), SLOT(onGotUserInfo(lastfm::User)) );

    connect( &ScrobbleService::instance(), SIGNAL(scrobblesCached(QList<lastfm::Track>)), SLOT(onScrobblesCached(QList<lastfm::Track>)));

    onSessionChanged( aApp->currentSession() );
}

ProfileWidget::~ProfileWidget()
{
    delete ui;
}

void
ProfileWidget::onSessionChanged( unicorn::Session* session )
{  
    onGotUserInfo( session->userInfo() );
}


void
ProfileWidget::onGotUserInfo( const lastfm::User& userDetails )
{
    changeUser( userDetails );
}

void
ProfileWidget::refresh()
{
    changeUser( aApp->currentSession()->userInfo() );
}

void
ProfileWidget::onCurrentChanged( int index )
{
    if ( index == 2 )
        refresh();
}

void
ProfileWidget::onGotLibraryArtists()
{
    lastfm::XmlQuery lfm;

    if ( lfm.parse( static_cast<QNetworkReply*>(sender())->readAll() ) )
    {
        int scrobblesPerDay = aApp->currentSession()->userInfo().scrobbleCount() / aApp->currentSession()->userInfo().dateRegistered().daysTo( QDateTime::currentDateTime() );
        int totalArtists = lfm["artists"].attribute( "total" ).toInt();

        ui->userBlurb->setText( tr( "You have %L1 artists in your library and on average listen to %L2 tracks per day." ).arg( totalArtists ).arg( scrobblesPerDay ) );
        ui->userBlurb->show();
    }
    else
    {
        qDebug() << lfm.parseError().message() << lfm.parseError().enumValue();
    }
}

void
ProfileWidget::changeUser( const lastfm::User& user )
{
    // Make sure we don't recieve any updates about the last session
    disconnect( this, SLOT(onGotLovedTracks()) );
    disconnect( this, SLOT(onGotTopOverallArtists()));
    disconnect( this, SLOT(onGotTopWeeklyArtists()));
    disconnect( this, SLOT(onGotLibraryArtists()));

    ui->avatar->setAlignment( Qt::AlignCenter );

    ui->avatar->setUser( user );
    ui->avatar->loadUrl( user.imageUrl( User::LargeImage, true ), HttpImageWidget::ScaleNone );
    ui->avatar->setHref( user.www() );

    ui->infoString->setText( user.getInfoString() );

    ui->scrobbles->setText( tr( "Scrobbles since %1" ).arg( user.dateRegistered().toString( "d MMMM yyyy" ) ) );

    m_scrobbleCount = user.scrobbleCount();
    setScrobbleCount();

    ui->name->setText( unicorn::Label::boldLinkStyle( unicorn::Label::anchor( user.www().toString(), user.name() ), Qt::black ) );

    connect( user.getLovedTracks( 1 ), SIGNAL(finished()), SLOT(onGotLovedTracks()) );
    connect( user.getTopArtists( "overall", 5, 1 ), SIGNAL(finished()), SLOT(onGotTopOverallArtists()));
    connect( user.getTopArtists( "7day", 5, 1 ), SIGNAL(finished()), SLOT(onGotTopWeeklyArtists()));

    connect( lastfm::Library::getArtists( user.name(), 1 ), SIGNAL(finished()), SLOT(onGotLibraryArtists()));
}


void
ProfileWidget::onGotTopWeeklyArtists()
{
    lastfm::XmlQuery lfm;

    if ( lfm.parse( qobject_cast<QNetworkReply*>(sender())->readAll() ) )
    {
        ui->weekFrame->setUpdatesEnabled( false );

        ui->weekFrame->layout()->takeAt( 0 )->widget()->deleteLater();

        QFrame* temp = new QFrame( this );
        ui->weekFrame->layout()->addWidget( temp );
        QVBoxLayout* layout = new QVBoxLayout( temp );
        layout->setContentsMargins( 0, 0, 0, 0 );
        layout->setSpacing( 0 );

        int maxPlays = lfm["topartists"]["artist"]["playcount"].text().toInt();

        foreach ( const lastfm::XmlQuery& artist, lfm["topartists"].children("artist") )
            layout->addWidget( new ProfileArtistWidget( artist, maxPlays, this ) );

        ui->weekFrame->setUpdatesEnabled( true );
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
        ui->overallFrame->setUpdatesEnabled( false );

        ui->overallFrame->layout()->takeAt( 0 )->widget()->deleteLater();

        QFrame* temp = new QFrame( this );
        ui->overallFrame->layout()->addWidget( temp );
        QVBoxLayout* layout = new QVBoxLayout( temp );
        layout->setContentsMargins( 0, 0, 0, 0 );
        layout->setSpacing( 0 );

        int maxPlays = lfm["topartists"]["artist"]["playcount"].text().toInt();

        foreach ( const lastfm::XmlQuery& artist, lfm["topartists"].children("artist") )
            layout->addWidget( new ProfileArtistWidget( artist, maxPlays, this ) );

        ui->overallFrame->setUpdatesEnabled( true );
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
        ui->lovedCount->setText( tr( "%L1" ).arg( lfm["lovedtracks"].attribute( "total" ).toInt() ) );
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
    ui->scrobbleCount->setText( QString( "%L1" ).arg( m_scrobbleCount ) );
}
