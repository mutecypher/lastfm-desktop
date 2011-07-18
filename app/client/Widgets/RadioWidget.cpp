

#include <QVBoxLayout>

#include <lastfm/RadioStation>
#include <lastfm/XmlQuery>

#include "lib/unicorn/StylableWidget.h"

#include "../Services/RadioService/RadioService.h"

#include "../Application.h"

#include "PlayableItemWidget.h"
#include "QuickStartWidget.h"

#include "RadioWidget.h"

RadioWidget::RadioWidget(QWidget *parent) :
    QWidget(parent)
{
    connect( aApp, SIGNAL(sessionChanged(unicorn::Session*)), SLOT(onSessionChanged(unicorn::Session*) ) );
}

void
RadioWidget::onSessionChanged( unicorn::Session* session )
{
    if ( !session->userInfo().name().isEmpty() )
    {
        // remove any previous layout
        if ( layout() )
        {
            for ( int i = layout()->count() - 1 ; i >= 0  ; --i )
                layout()->takeAt( i )->widget()->deleteLater();

            delete layout();
        }

        // new layout!
        QVBoxLayout* layout = new QVBoxLayout( this );

        QuickStartWidget* quickStartWidget = new QuickStartWidget();
        layout->addWidget( quickStartWidget );
        connect( quickStartWidget, SIGNAL(startRadio(RadioStation)), &RadioService::instance(), SLOT(play(RadioStation)));

        layout->addWidget( new PlayableItemWidget( tr( "My Library Radio" ), RadioStation::library( session->userInfo() ) ) );
        layout->addWidget( new PlayableItemWidget( tr( "My Mix Radio" ), RadioStation::mix( session->userInfo() ) ) );
        layout->addWidget( new PlayableItemWidget( tr( "My Recommended Radio" ), RadioStation::recommendations( session->userInfo() ) ) );

        layout->addWidget( ui.topArtists = new StylableWidget( this ) );
        layout->addWidget( ui.recentStations = new StylableWidget( this ) );

        // fetch top artists and recent stations
        connect( session->userInfo().getTopArtists( "3month", 5 ), SIGNAL(finished()), SLOT(onGotTopArtists()));
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

        lastfm::XmlQuery lfm = lastfm::ws::parse( qobject_cast<QNetworkReply*>(sender()) );

        foreach ( const lastfm::XmlQuery& artist, lfm["topartists"].children("artist") )
        {
            QString artistName = artist["name"].text();

            layout->addWidget( new PlayableItemWidget( tr( "%1 Similar Radio" ).arg( artistName ), RadioStation::similar( lastfm::Artist( artistName ) ) ) );
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

        lastfm::XmlQuery lfm = lastfm::ws::parse( qobject_cast<QNetworkReply*>(sender()) );

        foreach ( const lastfm::XmlQuery& station, lfm["recentstations"].children("station") )
            layout->addWidget( new PlayableItemWidget( station["name"].text(), RadioStation( station["url"].text() ) ) );
    }
    catch ( lastfm::ws::ParseError error )
    {
        qDebug() << error.message();
    }
}
