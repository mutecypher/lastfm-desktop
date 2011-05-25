

#include <QDebug>
#include <QTreeWidget>

#include <lastfm/User>
#include <lastfm/XmlQuery>
#include <lastfm/Tasteometer>

#include "../Application.h"
#include "Services/RadioService.h"
#include "../FriendsSortFilterProxyModel.h"
#include "../StationListModel.h"
#include "../Application.h"

#include "StationPanelWidget.h"
#include "ui_StationPanelWidget.h"

StationPanelWidget::StationPanelWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StationPanelWidget)
{
    ui->setupUi(this);

    m_friendsProxyModel = new FriendsSortFilterProxyModel( this );

    m_friendsProxyModel->setSourceModel( ui->friendsList->treeView()->model() );
    ui->friendsList->treeView()->setModel( m_friendsProxyModel );

    m_friendsProxyModel->setSelectionModel( ui->friendsList->treeView()->selectionModel() );

    connect( ui->friendsSort, SIGNAL(clicked()), SLOT(onFriendsSortClicked()) );

    connect( ui->friendsFilter, SIGNAL(textChanged(QString)), m_friendsProxyModel, SLOT(setFilterFixedString(QString)));

    connect( radio, SIGNAL(tuningIn(RadioStation)), SLOT(onTuningIn(RadioStation)));

    connect( aApp, SIGNAL(sessionChanged(unicorn::Session*)), SLOT(onSessionChanged(unicorn::Session*)) );

    // always start on the recent tab
    ui->stationTabWidget->setCurrentWidget( ui->recentTab );

    refresh();
}


StationPanelWidget::~StationPanelWidget()
{
    delete ui;
}


void
StationPanelWidget::onSessionChanged( unicorn::Session* newSession )
{
    if ( m_currentUser == User() )
        return;

    m_currentUser = User();

    refresh();
}

void
StationPanelWidget::refresh()
{
    if ( !User().name().isEmpty() )
    {
        ui->recentList->clear();
        ui->tagsList->clear();
        ui->friendsList->clear();
        ui->neighboursList->clear();
        ui->artistsList->clear();

        // start fetching things
        RadioStation yourLibrary = RadioStation::library( User() );
        yourLibrary.setTitle( "Your Library Radio" );
        ui->friendsList->addStation( yourLibrary, "You" );
        ui->friendsList->setTasteometerCompareScore( User().name(), 1 );

        connect( User().getRecentStations( 7 ), SIGNAL(finished()), SLOT(onGotRecentStations()) );
        connect( User().getFriends( true, 50 ), SIGNAL(finished()), SLOT(onGotFriends()));
        connect( User().getNeighbours( 50 ), SIGNAL(finished()), SLOT(onGotNeighbours()));
        connect( User().getTopArtists( "3month", 50 ), SIGNAL(finished()), SLOT(onGotArtists()));
        connect( RadioStation::library( User().name() ).getTagSuggestions( 50 ), SIGNAL(finished()), SLOT(onGotTags()));
    }
}

void
StationPanelWidget::onTuningIn( const RadioStation& station )
{
    ui->recentList->recentStation( station );
}


void
StationPanelWidget::onFriendsSortClicked()
{
    Qt::SortOrder sortOrder = Qt::AscendingOrder;

    if ( m_friendsProxyModel->sortRole() == StationListModel::TitleRole )
        m_friendsProxyModel->setSortRole( StationListModel::TimestampRole );
    else if ( m_friendsProxyModel->sortRole() == StationListModel::TimestampRole )
    {
        m_friendsProxyModel->setSortRole( StationListModel::TasteometerScoreRole );
        sortOrder = Qt::DescendingOrder;
    }
    else
        m_friendsProxyModel->setSortRole( StationListModel::TitleRole );

    m_friendsProxyModel->sort( 0, sortOrder );
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

        lastfm::User usersFriend = lastfm::User( user["name"].text() );
        RadioStation station = RadioStation::library( usersFriend );
        station.setTitle( tr("%1%2s Library Radio").arg( user["name"].text(), QChar( 0x2019 ) ) );
        ui->friendsList->addStation( station, track.toString() );
    }

    int page = lfm["friends"].attribute( "page" ).toInt();
    int totalPages = lfm["friends"].attribute( "totalPages" ).toInt();
    int perPage = lfm["friends"].attribute( "perPage" ).toInt();

    if ( page != totalPages )
        connect( User().getFriends( true, perPage, page + 1 ), SIGNAL(finished()), SLOT(onGotFriends()) );
}


void
StationPanelWidget::onGotTasteometerCompare()
{

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


