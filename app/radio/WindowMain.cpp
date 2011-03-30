
#include <QShortcut>
#include <QToolBar>
#include <QListWidgetItem>
#include <QMenuBar>
#include <QProcess>

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
    SETUP();

    m_actions->connectTriggers( this );

    ui->stationEdit->setHelpText( tr("Type an artist or tag and press play") );
    ui->stationEdit->setAttribute( Qt::WA_MacShowFocusRect, false );

    connect( ui->start, SIGNAL(clicked()), SLOT(onStartClicked()));
    connect( ui->stationEdit, SIGNAL(returnPressed()), ui->start, SLOT(click()));
    connect( ui->stationEdit, SIGNAL(textChanged(QString)), SLOT(onStationEditTextChanged(QString)));

    // fetch the recent radio stations
    lastfm::User currentUser;
    RadioStation library = RadioStation::library( currentUser );
    library.setTitle( tr("My Library") );
    ui->stations->addStation( library, tr("The music you know and love")  );

    RadioStation mix = RadioStation::mix( currentUser );
    mix.setTitle( tr( "My Mix Radio" ) );
    ui->stations->addStation( mix, tr("Your library + new music") );

    RadioStation recommendations = RadioStation::recommendations( currentUser );
    recommendations.setTitle( tr( "My Recomendation Radio" ) );
    ui->stations->addStation( recommendations, tr("New music from Last.fm") );

    RadioStation friends = RadioStation::friends( currentUser );
    friends.setTitle( tr( "My Friends%1 Radio" ).arg( QChar( 0x2019 ) ) );
    ui->stations->addStation( friends, tr("Music your friends like") );

    RadioStation neighbourhood = RadioStation::neighbourhood( currentUser );
    neighbourhood.setTitle( tr( "My Neighbours%1 Radio" ).arg( QChar( 0x2019 ) ) );
    ui->stations->addStation( neighbourhood, tr("Music from listeners like you") );

    connect( currentUser.getRecentStations( 10 ), SIGNAL(finished()), SLOT(onGotRecentStations()));

    connect( ui->back, SIGNAL(clicked()), SLOT(onBackClicked()) );
    ui->backGhost->setOrigin( ui->back );

    connect( ui->nowPlaying, SIGNAL(clicked()), SLOT(onNowPlayingClicked()) );
    ui->nowPlayingGhost->setOrigin( ui->nowPlaying );

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
    ON_ACTIONS_CHANGED();
}


void
WindowMain::onBackClicked()
{
    ui->details->setCurrentWidget( ui->quickstartPage );
}


void
WindowMain::onNowPlayingClicked()
{
    ui->details->setCurrentWidget( ui->detailsPage );
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

    if ( ui->stationEdit->text().length() )
    {
        StationSearch* s = new StationSearch();
        connect(s, SIGNAL(searchResult(RadioStation)), radio, SLOT(play(RadioStation)));
        s->startSearch(ui->stationEdit->text());
    }

    ui->stationEdit->clear();
}


void
WindowMain::onStationEditTextChanged( const QString& text )
{
    ui->start->setEnabled( text.count() > 0 );
}

void
WindowMain::onSpace()
{
    SPACE();
}


void
WindowMain::onPlayClicked( bool checked )
{
    PLAY_CLICKED();
}


void
WindowMain::onPlayTriggered( bool checked )
{
    PLAY_TRIGGERED();
}


void
WindowMain::onSkipClicked()
{
    SKIP_CLICKED();
}


void
WindowMain::onLoveClicked( bool loved )
{
    LOVE_CLICKED();
}

void
WindowMain::onLoveTriggered( bool loved )
{
    LOVE_TRIGGERED();
}

void
WindowMain::onBanClicked()
{
    BAN_CLICKED();
}

void
WindowMain::onBanFinished()
{
    BAN_FINISHED();
}


void
WindowMain::onInfoClicked()
{
#ifdef Q_OS_WIN32
    AllowSetForegroundWindow(ASFW_ANY);
#endif

#ifndef Q_OS_MAC
    QString path = qApp->applicationDirPath() + "/Last.fm Scrobbler";
#ifdef Q_OS_WIN
    path += ".exe";
    path.prepend("\"");
    path.append("\"");
#endif
    QProcess::startDetached( path );
#else
    FSRef appRef;
    LSFindApplicationForInfo( kLSUnknownCreator, CFSTR( "fm.last.audioscrobbler" ), NULL, &appRef, NULL );
    OSStatus status = LSOpenFSRef( &appRef, NULL );
#endif
}


void
WindowMain::onFilterClicked()
{
    ON_FILTER_CLICKED()
}


void
WindowMain::onEditClicked()
{
    ON_EDIT_CLICKED();
}

void
WindowMain::onRadioTick( qint64 tick )
{
    RADIO_TICK();
}


void
WindowMain::onTuningIn( const RadioStation& station )
{
    ON_TUNING_IN();

    ui->details->setCurrentWidget( ui->detailsPage );
}

QString userLibrary( const QString& user, const QString& artist )
{
    return Label::anchor( QString("http://www.last.fm/user/%1/library/music/%2").arg( user, artist ), user );
}


void
WindowMain::onTrackSpooled( const Track& track )
{
    TRACK_SPOOLED();

    lastfm::TrackContext context = track.context();

    if (context.values().count() > 0)
    {

        QString contextString;

        switch ( context.type() )
        {
        case lastfm::TrackContext::Artist:
            {
            switch ( context.values().count() )
                {
                default:
                case 1: contextString = tr( "Because you listen to %1" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ) ); break;
                case 2: contextString = tr( "Because you listen to %1, and %2" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ), Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ) ); break;
                case 3: contextString = tr( "Because you listen to %1, %2, and %3" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ) , Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ), Label::anchor( Artist( context.values().at(2) ).www().toString(), context.values().at(2) ) ); break;
                case 4: contextString = tr( "Because you listen to %1, %2, %3, and %4" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ), Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ), Label::anchor( Artist( context.values().at(2) ).www().toString(), context.values().at(2) ), Label::anchor( Artist( context.values().at(3) ).www().toString(), context.values().at(3) ) ); break;
                case 5: contextString = tr( "Because you listen to %1, %2, %3, %4, and %5" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ), Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ), Label::anchor( Artist( context.values().at(2) ).www().toString(), context.values().at(2) ), Label::anchor( Artist( context.values().at(3) ).www().toString(), context.values().at(3) ), Label::anchor( Artist( context.values().at(4) ).www().toString(), context.values().at(4) ) ); break;
                }
            }
            break;
        case lastfm::TrackContext::User:
            // Whitelist multi-user station
            if ( !radio->station().url().startsWith("lastfm://users/") )
                break;
        case lastfm::TrackContext::Friend:
        case lastfm::TrackContext::Neighbour:
            {
            switch ( context.values().count() )
                {
                default:
                case 1: contextString = tr( "From %2%1s library" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ) ); break;
                case 2: contextString = tr( "From %2, and %3%1s libraries" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ), userLibrary( context.values().at(1), track.artist().name() ) ); break;
                case 3: contextString = tr( "From %2, %3, and %4%1s libraries" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ), userLibrary( context.values().at(1), track.artist().name() ), userLibrary( context.values().at(2), track.artist().name() ) ); break;
                case 4: contextString = tr( "From %2, %3, %4, and %5%1s libraries" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ),userLibrary(  context.values().at(1), track.artist().name() ), userLibrary( context.values().at(2), track.artist().name() ), userLibrary( context.values().at(3), track.artist().name() ) ); break;
                case 5: contextString = tr( "From %2, %3, %4, %5, and %6%1s libraries" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ), userLibrary( context.values().at(1), track.artist().name() ), userLibrary( context.values().at(2), track.artist().name() ), userLibrary( context.values().at(3), track.artist().name() ), userLibrary( context.values().at(4), track.artist().name() ) ); break;
                }
            }
            break;
        }

        ui->context->setText( contextString );
    }
    else
        ui->context->clear();
}



void
WindowMain::onError( int error, const QVariant& errorText )
{
    ON_ERROR();
}


void
WindowMain::onStopped()
{
    ON_STOPPED();
}


void
WindowMain::onSwitch()
{
    emit aboutToHide();
    hide();
}

void
WindowMain::onGotRecentStations()
{
    lastfm::XmlQuery lfm = lastfm::XmlQuery( static_cast<QNetworkReply*>( sender() )->readAll() );

    foreach ( const lastfm::XmlQuery& station , lfm["recentstations"].children("station").mid( 0, 20 ) )
    {
        if ( ! (station["url"].text().contains("user/" + lastfm::ws::Username + "/") &&
             !station["url"].text().contains("/tag/") ) )
        {
            RadioStation recentStation( station["url"].text() );
            recentStation.setTitle( station["name"].text() );
            ui->stations->addStation( recentStation, tr("Recent") );
        }
    }
}

void
WindowMain::onGotEvents()
{
    GOT_EVENTS();
}

