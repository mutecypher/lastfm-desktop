
#include <QShortcut>
#include <QToolBar>
#include <QListWidgetItem>
#include <QMenuBar>

#include "WindowMacro.h"

#include "WindowMain.h"
#include "ui_WindowMain.h"

#include "widgets/TagFilterDialog.h"

#include "widgets/PlayableItemWidget.h"
#include "Actions.h"

#include "../Radio.h"
#include "../StationSearch.h"

#include <lastfm/XmlQuery>
#include <lastfm/User>

WindowMain::WindowMain( Actions& actions ) :
    unicorn::MainWindow(),
    ui(new Ui::WindowMain),
    m_actions( &actions )
{
    SETUP()

    //setWindowFlags( Qt::CustomizeWindowHint | Qt::FramelessWindowHint );

    connect( radio, SIGNAL(error(int,QVariant)), SLOT(onError(int, QVariant)));
    connect( radio, SIGNAL(stopped()), SLOT(onStopped()));

    ui->stationEdit->setHelpText( tr("Type an artist or tag and press play") );
    ui->stationEdit->setAttribute( Qt::WA_MacShowFocusRect, false );

    connect( radio, SIGNAL(tuningIn(RadioStation)), SLOT(onTuningIn(RadioStation)));

    connect( ui->start, SIGNAL(clicked()), SLOT(onStartClicked()));
    connect( ui->stationEdit, SIGNAL(returnPressed()), ui->start, SLOT(click()));

    connect( ui->recent, SIGNAL(clicked()), SLOT(onRecentClicked()));
    connect( ui->library, SIGNAL(clicked()), SLOT(onLibraryClicked()));
    connect( ui->mix, SIGNAL(clicked()), SLOT(onMixClicked()));
    connect( ui->recommended, SIGNAL(clicked()), SLOT(onRecommendedClicked()));
    connect( ui->friends, SIGNAL(clicked()), SLOT(onFriendsClicked()));
    connect( ui->neighbours, SIGNAL(clicked()), SLOT(onNeighboursClicked()));

    // fetch the recent radio stations
    lastfm::User currentUser;
    connect( currentUser.getRecentStations( 20 ), SIGNAL(finished()), SLOT(onGotRecentStations()));
    createLibraryStations();
    createMixStations();
    createRecommendedStations();
    createFriendsStations();
    createNeighboursStations();


    // always start on the recent tab
    ui->library->click();

    ui->details->setCurrentWidget( ui->quickstartPage );

    addDragHandleWidget( ui->controlFrame );
}


void
WindowMain::addWinThumbBarButtons( QList<QAction*>& thumbButtonActions )
{
    thumbButtonActions.append( m_actions->m_loveAction );
    thumbButtonActions.append( m_actions->m_banAction );
    thumbButtonActions.append( m_actions->m_playAction );
    thumbButtonActions.append( m_actions->m_skipAction );
}


void
WindowMain::onActionsChanged()
{
    ui->love->setChecked( m_actions->m_loveAction->isChecked() );
    ui->ban->setChecked( m_actions->m_banAction->isChecked() );
    ui->play->setChecked( m_actions->m_playAction->isChecked() );
    ui->skip->setChecked( m_actions->m_skipAction->isChecked() );

    ui->love->setEnabled( m_actions->m_loveAction->isEnabled() );
    ui->ban->setEnabled( m_actions->m_banAction->isEnabled() );
    ui->play->setEnabled( m_actions->m_playAction->isEnabled() );
    ui->skip->setEnabled( m_actions->m_skipAction->isEnabled() );
}


WindowMain::~WindowMain()
{
    delete ui;
}


void
WindowMain::onStartClicked()
{
    QString trimmedText = ui->stationEdit->text().trimmed();
    if( trimmedText.startsWith("lastfm://")) {
        radio->play( RadioStation( trimmedText ) );
        return;
    }

    if (ui->stationEdit->text().length()) {
        StationSearch* s = new StationSearch();
        connect(s, SIGNAL(searchResult(RadioStation)), radio, SLOT(play(RadioStation)));
        s->startSearch(ui->stationEdit->text());
    }

    ui->stationEdit->clear();
}


void
WindowMain::onSpace()
{
    SPACE()
}


void
WindowMain::onPlayClicked( bool checked )
{
    PLAY_CLICKED()
}


void
WindowMain::onSkipClicked()
{
    SKIP_CLICKED()
}

void
WindowMain::onLoveClicked( bool loved )
{
    LOVE_CLICKED()
}

void
WindowMain::onLoveTriggered()
{
    LOVE_TRIGGERED()
}

void
WindowMain::onBanClicked()
{
    BAN_CLICKED()
}

void
WindowMain::onBanFinished()
{
    BAN_FINISHED()
}


void
WindowMain::onFilterClicked()
{
    ON_FILTER_CLICKED()
}


void
WindowMain::onEditClicked()
{
    ON_EDIT_CLICKED()
}

void
WindowMain::onRadioTick( qint64 tick )
{
    RADIO_TICK()
}


void
WindowMain::onTuningIn( const RadioStation& station )
{
    ON_TUNING_IN()

    ui->details->setCurrentWidget( ui->detailsPage );

    ui->recentList->recentStation( station );
}


void
WindowMain::onTrackSpooled( const Track& track )
{
    TRACK_SPOOLED()
}


void
WindowMain::onError(int error, const QVariant& errorText)
{
    ui->radioTitle->setText( errorText.toString() + ": " + QString::number(error) );
}


void
WindowMain::onStopped()
{
    ui->radioTitle->setText( tr("Radio Title") );
}


void
WindowMain::onSwitch()
{
    emit aboutToHide();
    hide();
}

void
WindowMain::onRecentClicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void
WindowMain::onLibraryClicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void
WindowMain::onMixClicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void
WindowMain::onRecommendedClicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void
WindowMain::onFriendsClicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

void
WindowMain::onNeighboursClicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}

void
WindowMain::onGotRecentStations()
{
    lastfm::XmlQuery lfm = lastfm::XmlQuery( static_cast<QNetworkReply*>( sender() )->readAll() );

    foreach ( const lastfm::XmlQuery& station , lfm.children("station").mid( 0, 20 ) )
    {
        RadioStation recentStation( station["url"].text() );
        recentStation.setTitle( station["name"].text() );
        ui->recentList->addStation( recentStation );
    }
}

void
WindowMain::createLibraryStations()
{
    RadioStation station = RadioStation::library( lastfm::User() );
    station.setTitle( tr("My Library Radio") );
    ui->libraryList->addStation( station );

    connect( station.getSampleArtists( 20 ), SIGNAL(finished()), SLOT(onGotLibraryArtists()));
}


void
WindowMain::onGotLibraryArtists()
{
    lastfm::XmlQuery lfm = lastfm::XmlQuery( static_cast<QNetworkReply*>( sender() )->readAll() );

    //ui->libraryLayout->addWidget( new QLabel( tr("Containing these artists"), this ) );

    foreach ( lastfm::XmlQuery artist, lfm.children("artist").mid( 0, 20 ) )
    {
        RadioStation station = RadioStation::similar( lastfm::Artist( artist["name"].text() ) );
        station.setTitle( tr("%1 Radio").arg( artist["name"].text() ) );
        ui->libraryList->addStation( station );
    }
}


void
WindowMain::createMixStations()
{
    RadioStation station = RadioStation::mix( lastfm::User() );
    station.setTitle( tr("My Mix Radio") );
    ui->mixList->addStation( station );

    //ui->mixLayout->addWidget( new QLabel( tr("Containing these artists"), this ) );

    connect( station.getSampleArtists( 20 ), SIGNAL(finished()), SLOT(onGotMixArtists()));
}


void
WindowMain::onGotMixArtists()
{
    lastfm::XmlQuery lfm = lastfm::XmlQuery( static_cast<QNetworkReply*>( sender() )->readAll() );

    foreach ( lastfm::XmlQuery artist, lfm.children("artist").mid( 0, 20 ) )
    {
        RadioStation station = RadioStation::similar( lastfm::Artist( artist["name"].text() ) );
        station.setTitle( tr("%1 Radio").arg( artist["name"].text() ) );
        ui->mixList->addStation( station );
    }
}


void
WindowMain::createRecommendedStations()
{
    RadioStation station = RadioStation::recommendations( lastfm::User() );
    station.setTitle( tr("My Recommended Radio") );
    ui->recList->addStation( station );

    connect( station.getSampleArtists( 20 ), SIGNAL(finished()), SLOT(onGotRecommendedArtists()));
}

void
WindowMain::onGotRecommendedArtists()
{
    lastfm::XmlQuery lfm = lastfm::XmlQuery( static_cast<QNetworkReply*>( sender() )->readAll() );

    //ui->recLayout->addWidget( new QLabel( tr("Containing these artists"), this ) );

    foreach ( lastfm::XmlQuery artist, lfm.children("artist").mid( 0, 20 ) )
    {
        RadioStation station = RadioStation::similar( lastfm::Artist( artist["name"].text() ) );
        station.setTitle( tr("%1 Radio").arg( artist["name"].text() ) );
        ui->recList->addStation( station );
    }
}

void
WindowMain::createFriendsStations()
{
    RadioStation station = RadioStation::friends( lastfm::User() );
    station.setTitle( tr("My Friends%1 Radio").arg( QChar( 0x2019 ) ) );
    ui->friendsList->addStation( station );

    connect( User().getFriendsListeningNow( 50 ), SIGNAL(finished()), SLOT(onGotFriendsListeningNow()));
}

void
WindowMain::onGotFriendsListeningNow()
{
    lastfm::XmlQuery lfm = lastfm::XmlQuery( static_cast<QNetworkReply*>( sender() )->readAll() );

    //ui->friendsLayout->addWidget( new QLabel( tr("Containing these users"), this ) );

    foreach ( lastfm::XmlQuery user, lfm.children("user") )
    {
        RadioStation station = RadioStation::library( lastfm::User( user["name"].text() ) );
        station.setTitle( tr("%1%2s Library Radio").arg( user["name"].text(), QChar( 0x2019 ) ) );
        ui->friendsList->addStation( station );
    }
}


void
WindowMain::createNeighboursStations()
{
    RadioStation station = RadioStation::neighbourhood( lastfm::User() );
    station.setTitle( tr("My Neighbours%1 Radio").arg( QChar( 0x2019 ) ) );
    ui->neighboursList->addStation( station );

    connect( User().getNeighbours( 50 ), SIGNAL(finished()), SLOT(onGotNeighbours()));
}


void
WindowMain::onGotNeighbours()
{
    lastfm::XmlQuery lfm = lastfm::XmlQuery( static_cast<QNetworkReply*>( sender() )->readAll() );

    //ui->neighboursLayout->addWidget( new QLabel( tr("Containing these users"), this ) );

    foreach ( lastfm::XmlQuery user, lfm.children("user") )
    {
        RadioStation station = RadioStation::library( lastfm::User( user["name"].text() ) );
        station.setTitle( tr("%1%2s Library Radio").arg( user["name"].text(), QChar( 0x2019 ) ) );
        ui->neighboursList->addStation( station );
    }
}
