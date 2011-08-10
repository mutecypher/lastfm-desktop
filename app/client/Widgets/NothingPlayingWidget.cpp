
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "../Application.h"

#include "QuickStartWidget.h"

#include "lib/unicorn/widgets/Label.h"

#include "NothingPlayingWidget.h"

NothingPlayingWidget::NothingPlayingWidget( QWidget* parent )
    :StylableWidget( parent )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    layout->addWidget( ui.hello = new Label( tr( "Hello! Let's start a RadioStation..." ) ) );
    ui.hello->setObjectName( "hello" );

    layout->addWidget( ui.type = new QLabel( tr( "Type an artist or genre into the box and press play, or visit the Radio tab for more advanced options." ) ) );
    ui.type->setObjectName( "type" );
    ui.type->setWordWrap( true );

    layout->addWidget( ui.quickStart = new QuickStartWidget( this ) );
    ui.quickStart->setObjectName( "quickStart" );

    layout->addWidget( ui.split = new QLabel( tr( "OR" ) ) );
    ui.split->setObjectName( "split" );

    layout->addWidget( ui.scrobble = new QLabel( tr( "Scrobble from your music player." ) ) );
    ui.scrobble->setObjectName( "scrobble" );

    layout->addWidget( ui.scrobbleExplain = new QLabel( tr( "Effortlessly keep your Last.fm profile updated with the tracks you're playing on other media players. You can see more about the track you're playing on the scrobbles tab." ) ) );
    ui.scrobbleExplain->setObjectName( "scrobbleExplain" );
    ui.scrobbleExplain->setWordWrap( true );

    layout->addWidget( ui.clickPlayers = new QLabel( tr( "Click on a player below to launch it." ) ) );
    ui.clickPlayers->setObjectName( "clickPlayers" );

    layout->addWidget( ui.players = new StylableWidget );
    ui.players->setObjectName( "players" );
    QHBoxLayout* hl = new QHBoxLayout( ui.players );
    hl->setContentsMargins( 0, 0, 0, 0 );
    hl->setSpacing( 0 );

    hl->addStretch( 1 );
    hl->addWidget( ui.itunes = new QPushButton( this ) );
    ui.itunes->setObjectName( "itunes" );
    hl->addWidget( ui.wmp = new QPushButton( this ) );
    ui.wmp->setObjectName( "wmp" );
    hl->addWidget( ui.winamp = new QPushButton( this ) );
    ui.winamp->setObjectName( "winamp" );
    hl->addWidget( ui.foobar = new QPushButton( this ) );
    ui.foobar->setObjectName( "foobar" );
    hl->addStretch( 1 );

    layout->addStretch( 1 );

    connect( aApp, SIGNAL(sessionChanged(unicorn::Session*)), SLOT(onSessionChanged(unicorn::Session*)) );
}


void
NothingPlayingWidget::onSessionChanged( unicorn::Session* session )
{
    if ( !session->userInfo().name().isEmpty() )
        ui.hello->setText( tr( "Hello, %1! Let's start a RadioStation..." ).arg( session->userInfo().name() ) );
}
