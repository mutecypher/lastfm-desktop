

#include <QDebug>
#include <QSortFilterProxyModel>
#include <QTreeWidget>

#include <lastfm/User>
#include <lastfm/XmlQuery>

#include "../Radio.h"
#include "../FriendsSortFilterProxyModel.h"
#include "../StationListModel.h"

#include "StationPanelWidget.h"
#include "ui_StationPanelWidget.h"

StationPanelWidget::StationPanelWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StationPanelWidget)
{
    ui->setupUi(this);

    m_friendsProxyModel = new FriendsSortFilterProxyModel( *ui->friendsList->treeView()->selectionModel(), this );

    m_friendsProxyModel->setSourceModel( ui->friendsList->treeView()->model() );
    ui->friendsList->treeView()->setModel( m_friendsProxyModel );

    connect( ui->friendsSort, SIGNAL(clicked()), SLOT(onFriendsSortClicked()) );

    connect( ui->friendsFilter, SIGNAL(textChanged(QString)), m_friendsProxyModel, SLOT(setFilterFixedString(QString)));

    connect( ui->recent, SIGNAL(clicked()), SLOT(onRecentClicked()));
    connect( ui->friends, SIGNAL(clicked()), SLOT(onFriendsClicked()));
    connect( ui->neighbours, SIGNAL(clicked()), SLOT(onNeighboursClicked()));
    connect( ui->artists, SIGNAL(clicked()), SLOT(onArtistsClicked()));
    connect( ui->tags, SIGNAL(clicked()), SLOT(onTagsClicked()));

    connect( radio, SIGNAL(tuningIn(RadioStation)), SLOT(onTuningIn(RadioStation)));

    // start fetching things
    connect( User().getRecentStations( 20 ), SIGNAL(finished()), SLOT(onGotRecentStations()) );
    connect( User().getFriends( true, 50 ), SIGNAL(finished()), SLOT(onGotFriends()));
    connect( User().getNeighbours( 50 ), SIGNAL(finished()), SLOT(onGotNeighbours()));
    connect( User().getTopArtists( "3month", 50 ), SIGNAL(finished()), SLOT(onGotArtists()));
    connect( RadioStation::library( User().name() ).getTagSuggestions( 50 ), SIGNAL(finished()), SLOT(onGotTags()));


    // always start on the recent tab
    ui->recent->click();
}

StationPanelWidget::~StationPanelWidget()
{
    delete ui;
}
	
void
StationPanelWidget::onTuningIn( const RadioStation& station )
{
    ui->recentList->recentStation( station );
}

void
StationPanelWidget::onRecentClicked()
{
    ui->stackedWidget->setCurrentWidget( ui->recentPage );
}


void
StationPanelWidget::onFriendsClicked()
{
    ui->stackedWidget->setCurrentWidget( ui->friendsPage );
}

void
StationPanelWidget::onNeighboursClicked()
{
    ui->stackedWidget->setCurrentWidget( ui->neighboursPage );
}

void
StationPanelWidget::onArtistsClicked()
{
    ui->stackedWidget->setCurrentWidget( ui->artistsPage );
}

void
StationPanelWidget::onTagsClicked()
{
    ui->stackedWidget->setCurrentWidget( ui->tagsPage );
}


void
StationPanelWidget::onFriendsSortClicked()
{
    if ( m_friendsProxyModel->sortRole() == StationListModel::TitleRole )
        m_friendsProxyModel->setSortRole( StationListModel::TimestampRole );
    else
        m_friendsProxyModel->setSortRole( StationListModel::TitleRole );
    m_friendsProxyModel->sort( 0 );
}


void
StationPanelWidget::onGotRecentStations()
{
    lastfm::XmlQuery lfm = lastfm::XmlQuery( static_cast<QNetworkReply*>( sender() )->readAll() );

    foreach ( const lastfm::XmlQuery& station , lfm.children("station").mid( 0, 20 ) )
    {
        RadioStation recentStation( station["url"].text() );
        recentStation.setTitle( station["name"].text() );
        ui->recentList->addStation( recentStation, "" );
    }
}


void
StationPanelWidget::onGotFriends()
{
    lastfm::XmlQuery lfm = lastfm::XmlQuery( static_cast<QNetworkReply*>( sender() )->readAll() );

    foreach ( lastfm::XmlQuery user, lfm.children("user") )
    {
        Track track;
        MutableTrack( track ).setArtist( user["recenttrack"]["artist"]["name"].text() );
        MutableTrack( track ).setTitle( user["recenttrack"]["name"].text() );

        RadioStation station = RadioStation::library( lastfm::User( user["name"].text() ) );
        station.setTitle( tr("%1%2s Library Radio").arg( user["name"].text(), QChar( 0x2019 ) ) );
        ui->friendsList->addStation( station, user["realname"].text() + " - " + track.toString() );
    }

    int page = lfm["friends"].attribute( "page" ).toInt();
    int totalPages = lfm["friends"].attribute( "totalPages" ).toInt();
    int perPage = lfm["friends"].attribute( "perPage" ).toInt();

    if ( page != totalPages )
        connect( User().getFriends( true, perPage, page + 1 ), SIGNAL(finished()), SLOT(onGotFriends()) );
}


void
StationPanelWidget::onGotNeighbours()
{
    lastfm::XmlQuery lfm = lastfm::XmlQuery( static_cast<QNetworkReply*>( sender() )->readAll() );

    foreach ( lastfm::XmlQuery user, lfm.children("user") )
    {
        RadioStation station = RadioStation::library( lastfm::User( user["name"].text() ) );
        station.setTitle( tr("%1%2s Library Radio").arg( user["name"].text(), QChar( 0x2019 ) ) );
        ui->neighboursList->addStation( station, "" );
    }
}


void
StationPanelWidget::onGotArtists()
{
    lastfm::XmlQuery lfm = lastfm::XmlQuery( static_cast<QNetworkReply*>( sender() )->readAll() );

    foreach ( lastfm::XmlQuery artist, lfm.children("artist").mid( 0, 20 ) )
    {
        RadioStation station = RadioStation::similar( lastfm::Artist( artist["name"].text() ) );
        station.setTitle( tr("%1 Radio").arg( artist["name"].text() ) );
        ui->artistsList->addStation( station, tr("%1 plays in the last 3 months").arg( artist["playcount"].text() ) );
    }
}

void
StationPanelWidget::onGotTags()
{
    lastfm::XmlQuery lfm = lastfm::XmlQuery( static_cast<QNetworkReply*>( sender() )->readAll() );

    foreach ( lastfm::XmlQuery suggestion, lfm["suggestions"].children("suggestion").mid( 0, 20 ) )
    {
        RadioStation station = RadioStation::tag( lastfm::Tag( suggestion["tag"]["name"].text() ) );
        station.setTitle( tr("%1 Tag Radio").arg( suggestion["tag"]["name"].text() ) );
        ui->tagsList->addStation( station, "" );
    }
}


