

#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>

#include <lastfm/RadioStation>
#include <lastfm/XmlQuery>

#include "lib/unicorn/StylableWidget.h"

#include "../Application.h"

#include "PlayableItemWidget.h"
#include "QuickStartWidget.h"

#include "RadioWidget.h"

RadioWidget::RadioWidget(QWidget *parent)
    :QWidget( parent )
{
    ui.main = 0;

    QVBoxLayout* mainLayout = new QVBoxLayout( this );
    mainLayout->setContentsMargins( 0, 0, 0, 0 );
    mainLayout->setSpacing( 0 );

    connect( aApp, SIGNAL(sessionChanged(unicorn::Session*)), SLOT(onSessionChanged(unicorn::Session*) ) );
    connect( &RadioService::instance(), SIGNAL(tuningIn(RadioStation)), SLOT(onTuningIn(RadioStation) ) );
}

void
RadioWidget::onSessionChanged( unicorn::Session* session )
{
    if ( !session->userInfo().name().isEmpty() )
    {
        // remove any previous layout
        QLayoutItem* li = layout()->takeAt( 0 );
        if ( li )
            delete li->widget();

        layout()->addWidget( ui.main = new QWidget( this ) );
        QVBoxLayout* layout = new QVBoxLayout( ui.main );
        layout->setContentsMargins( 0, 0, 0, 0 );
        layout->setSpacing( 0 );

        QuickStartWidget* quickStartWidget = new QuickStartWidget();
        layout->addWidget( quickStartWidget );
        connect( quickStartWidget, SIGNAL(startRadio(RadioStation)), &RadioService::instance(), SLOT(play(RadioStation)));

        {
            QLabel* title = new QLabel( tr("Personal Stations"), this );
            layout->addWidget( title );
            title->setObjectName( "title" );
            layout->addWidget( ui.personal = new StylableWidget( this ) );
            ui.personal->setObjectName( "section" );
            QVBoxLayout* personalLayout = new QVBoxLayout( ui.personal );
            personalLayout->setContentsMargins( 0, 0, 0, 0 );
            personalLayout->setSpacing( 0 );
            personalLayout->addWidget( ui.library = new PlayableItemWidget( RadioStation::library( session->userInfo() ), tr( "My Library Radio" ), tr( "Music you know and love" ) ) );
            ui.library->setObjectName( "library" );
            personalLayout->addWidget( ui.mix = new PlayableItemWidget( RadioStation::mix( session->userInfo() ), tr( "My Mix Radio" ), tr( "Your library plus new music" ) ) );
            ui.mix->setObjectName( "mix" );
            personalLayout->addWidget( ui.rec = new PlayableItemWidget( RadioStation::recommendations( session->userInfo() ), tr( "My Recommended Radio" ), tr( "New music from Last.fm" ) ) );
            ui.rec->setObjectName( "rec" );
        }

        {
            QLabel* title = new QLabel( tr("Network Stations"), this ) ;
            layout->addWidget( title );
            title->setObjectName( "title" );
            layout->addWidget( ui.network = new StylableWidget( this ) );
            ui.network->setObjectName( "section" );
            QVBoxLayout* networkLayout = new QVBoxLayout( ui.network );
            networkLayout->setContentsMargins( 0, 0, 0, 0 );
            networkLayout->setSpacing( 0 );
            networkLayout->addWidget( ui.friends = new PlayableItemWidget( RadioStation::friends( session->userInfo() ), tr( "My Friends' Radio" ), tr( "Music your friends like" ) ) );
            ui.friends->setObjectName( "friends" );
            networkLayout->addWidget( ui.neighbours = new PlayableItemWidget( RadioStation::neighbourhood( session->userInfo() ), tr( "My Neighbours' Radio" ), tr ( "Music from listeners you like" ) ) );
            ui.neighbours->setObjectName( "neighbours" );
        }

        QLabel* title = new QLabel( tr("Recent Stations"), this ) ;
        layout->addWidget( title );
        title->setObjectName( "title" );
        layout->addWidget( ui.recentStations = new StylableWidget( this ) );
        ui.recentStations->setObjectName( "section" );

        //layout->addWidget( ui.topArtists = new StylableWidget( this ) );

        // fetch top artists and recent stations
        //connect( session->userInfo().getTopArtists( "3month", 5 ), SIGNAL(finished()), SLOT(onGotTopArtists()));
        connect( session->userInfo().getRecentStations( 5 ), SIGNAL(finished()), SLOT(onGotRecentStations()));

        layout->addStretch( 1 );
    }
}

void
RadioWidget::onGotTopArtists()
{
    try
    {
        QVBoxLayout* layout = new QVBoxLayout( ui.topArtists );
        layout->setContentsMargins( 0, 0, 0, 0 );
        layout->setSpacing( 0 );

        lastfm::XmlQuery lfm = lastfm::ws::parse( qobject_cast<QNetworkReply*>(sender()) );

        foreach ( const lastfm::XmlQuery& artist, lfm["topartists"].children("artist") )
        {
            QString artistName = artist["name"].text();
            PlayableItemWidget* item = new PlayableItemWidget( RadioStation::similar( lastfm::Artist( artistName ) ), tr( "%1 Similar Radio" ).arg( artistName ) );
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
RadioWidget::onGotRecentStations()
{
    try
    {
        QVBoxLayout* layout = new QVBoxLayout( ui.recentStations );
        layout->setContentsMargins( 0, 0, 0, 0 );
        layout->setSpacing( 0 );

        lastfm::XmlQuery lfm = lastfm::ws::parse( qobject_cast<QNetworkReply*>(sender()) );

        foreach ( const lastfm::XmlQuery& station, lfm["recentstations"].children("station") )
        {
            PlayableItemWidget* item = new PlayableItemWidget( RadioStation( station["url"].text() ), station["name"].text() );
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
RadioWidget::onTuningIn( const RadioStation& station )
{
    // insert at the front of the list

    if ( ui.recentStations->layout() )
    {
        PlayableItemWidget* item = new PlayableItemWidget( station, station.title() );
        item->setObjectName( "station" );
        qobject_cast<QBoxLayout*>(ui.recentStations->layout())->insertWidget( 0, item );

        // if it exists already remove it or remove the last one
        bool removed = false;

        for ( int i = 1 ; i < ui.recentStations->layout()->count() ; ++i )
        {
            if ( station.url() == qobject_cast<PlayableItemWidget*>(ui.recentStations->layout()->itemAt( i )->widget())->station().url() )
            {
                QWidget* taken = ui.recentStations->layout()->takeAt( i )->widget();
                taken->deleteLater();
                removed = true;
                break;
            }
        }

        if ( !removed )
        {
            QWidget* taken = ui.recentStations->layout()->takeAt( ui.recentStations->layout()->count() - 1 )->widget();
            taken->deleteLater();
        }
    }
}
