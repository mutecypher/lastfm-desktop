#include <QShortcut>
#include <QToolBar>
#include <QListWidgetItem>
#include <QMenuBar>
#include <QProcess>
#include <QTcpSocket>

#ifdef Q_OS_MAC
    #include <CoreServices/CoreServices.h>
#endif

#include "RadioWidget.h"
#include "ui_RadioWidget.h"

#include "TagFilterDialog.h"

#include "PlayableItemWidget.h"

#include "../Application.h"
#include "../Radio.h"
#include "../StationSearch.h"

#include <lastfm/XmlQuery>
#include <lastfm/User>

#ifdef Q_OS_MAC
#include "ApplicationServices/ApplicationServices.h"
#endif


RadioWidget::RadioWidget()
    :ui(new Ui::RadioWidget)
{
    ui->setupUi(this);

    // If the actions are triggered we should do something
    // love is dealt with by the application
    //connect( aApp->loveAction(), SIGNAL(triggered(bool)), SLOT(onLoveClicked(bool)) );
    connect( aApp->banAction(), SIGNAL(triggered(bool)), SLOT(onBanClicked()) );
    connect( aApp->playAction(), SIGNAL(triggered(bool)), SLOT(onPlayClicked(bool)) );
    connect( aApp->skipAction(), SIGNAL(triggered(bool)), SLOT(onSkipClicked()) );

    // make sure this widget updates if the actions are changed elsewhere
    connect( aApp->loveAction(), SIGNAL(changed()), SLOT(onActionsChanged()) );
    connect( aApp->banAction(), SIGNAL(changed()), SLOT(onActionsChanged()) );
    connect( aApp->playAction(), SIGNAL(changed()), SLOT(onActionsChanged()) );
    connect( aApp->skipAction(), SIGNAL(changed()), SLOT(onActionsChanged()) );

    // if our buttons are pressed we should trigger the actions
    connect( ui->love, SIGNAL(clicked()), aApp->loveAction(), SLOT(trigger()));
    connect( ui->ban, SIGNAL(clicked()), aApp->banAction(), SLOT(trigger()));
    connect( ui->play, SIGNAL(clicked()), aApp->playAction(), SLOT(trigger()));
    connect( ui->skip, SIGNAL(clicked()), aApp->skipAction(), SLOT(trigger()));

    ui->love->setToolTip( ui->love->isChecked() ? tr("Unlove") : tr("Love") );
    ui->ban->setToolTip( tr("Ban") );
    ui->info->setToolTip( tr("Info") );
    ui->play->setToolTip( ui->play->isChecked() ? tr("Pause") : tr("Play") );
    ui->skip->setToolTip( tr("Skip") );

    connect( radio, SIGNAL(trackSpooled(Track)), SLOT(onTrackSpooled(Track)) );
    connect( radio, SIGNAL(tick(qint64)), SLOT(onRadioTick(qint64)));
    connect( radio, SIGNAL(stopped()), SLOT(onStopped()));
    connect( radio, SIGNAL(tuningIn(RadioStation)), SLOT(onTuningIn(RadioStation)));
    connect( radio, SIGNAL(error(int,QVariant)), SLOT(onError(int, QVariant)));

    ui->trackTitle->setOpenExternalLinks( true);
    ui->album->setOpenExternalLinks( true);
    ui->context->setOpenExternalLinks( true);

    ui->volumeSlider->setAudioOutput( radio->audioOutput() );
    ui->volumeSlider->setMuteVisible( false );

    new QShortcut( QKeySequence( Qt::CTRL + Qt::Key_M ), this, SLOT(onSwitch()));
    new QShortcut( QKeySequence( Qt::Key_Space ), this, SLOT(onSpace()));

    ui->stationEdit->setHelpText( tr("Type an artist or tag and press play") );
    ui->stationEdit->setAttribute( Qt::WA_MacShowFocusRect, false );

    connect( ui->start, SIGNAL(clicked()), SLOT(onStartClicked()));
    connect( ui->stationEdit, SIGNAL(returnPressed()), ui->start, SLOT(click()));
    connect( ui->stationEdit, SIGNAL(textChanged(QString)), SLOT(onStationEditTextChanged(QString)));

    connect( ui->library, SIGNAL(clicked()), SLOT(onLibraryClicked()) );
    connect( ui->mix, SIGNAL(clicked()), SLOT(onMixClicked()) );
    connect( ui->recommended, SIGNAL(clicked()), SLOT(onRecomendedClicked()) );

    connect( ui->back, SIGNAL(clicked()), SLOT(onBackClicked()) );
    ui->backGhost->setOrigin( ui->back );

    connect( ui->nowPlaying, SIGNAL(clicked()), SLOT(onNowPlayingClicked()) );
    ui->nowPlayingGhost->setOrigin( ui->nowPlaying );

    ui->nowPlaying->hide();

    ui->details->setCurrentWidget( ui->quickstartPage );
}


void
RadioWidget::onLibraryClicked()
{
    radio->play( RadioStation::library( User().name() ) );
}


void
RadioWidget::onMixClicked()
{
    radio->play( RadioStation::mix( User().name() ) );
}


void
RadioWidget::onRecomendedClicked()
{
    radio->play( RadioStation::recommendations( User().name() ) );
}


void
RadioWidget::onActionsChanged()
{
   ui->love->setChecked( aApp->loveAction()->isChecked() );
   ui->ban->setChecked( aApp->banAction()->isChecked() );
   ui->play->setChecked( aApp->playAction()->isChecked() );
   ui->skip->setChecked( aApp->skipAction()->isChecked() );

   ui->love->setEnabled( aApp->loveAction()->isEnabled() );
   ui->ban->setEnabled( aApp->banAction()->isEnabled() );
   ui->play->setEnabled( aApp->playAction()->isEnabled() );
   ui->skip->setEnabled( aApp->skipAction()->isEnabled() );

   ui->love->setToolTip( ui->love->isChecked() ? tr("Unlove") : tr("Love") );
   ui->ban->setToolTip( tr("Ban") );
   ui->info->setToolTip( tr("Info") );
   ui->play->setToolTip( ui->play->isChecked() ? tr("Pause") : tr("Play") );
   ui->skip->setToolTip( tr("Skip") );

   ui->love->setText( ui->love->isChecked() ? tr("Unlove") : tr("Love") );
   ui->ban->setText( tr("Ban") );
   ui->info->setText( tr("Info") );
   ui->play->setText( ui->play->isChecked() ? tr("Pause") : tr("Play") );
   ui->skip->setText( tr("Skip") );
}


void
RadioWidget::onBackClicked()
{
    ui->details->setCurrentWidget( ui->quickstartPage );
}


void
RadioWidget::onNowPlayingClicked()
{
    ui->details->setCurrentWidget( ui->detailsPage );
}


RadioWidget::~RadioWidget()
{
    delete ui;
}


void
RadioWidget::onStartClicked()
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
RadioWidget::onStationEditTextChanged( const QString& text )
{
    ui->start->setEnabled( text.count() > 0 );
}

void
RadioWidget::onSpace()
{
    aApp->playAction()->trigger();
}


void
RadioWidget::onPlayClicked( bool checked )
{
    if ( checked )
    {
        if ( radio->state() == Radio::Stopped )
            radio->play( RadioStation( "" ) );
        else
        {
            radio->resume();
        }
    }
    else
    {
        radio->pause();
    }
}


void
RadioWidget::onPlayTriggered( bool checked )
{
    if ( checked )
    {
        if ( radio->state() != Radio::Stopped )
            setWindowTitle( QString( "Last.fm Radio - %1 - %2" ).arg( radio->station().title(), radio->currentTrack().toString() ) );
    }
    else
        setWindowTitle( QString( "Last.fm Radio - %1" ).arg( radio->station().title() ) );
}


void
RadioWidget::onSkipClicked()
{
    radio->skip();
}


void
RadioWidget::onLoveClicked( bool loved )
{
    MutableTrack track( radio->currentTrack() );

    if ( loved )
        track.love();
    else
        track.unlove();

    connect( track.signalProxy(), SIGNAL(loveToggled(bool)), ui->love, SLOT(setChecked(bool)));
}

void
RadioWidget::onLoveTriggered( bool loved )
{
    ui->love->setChecked( loved );
    onLoveClicked( loved );
}

void
RadioWidget::onBanClicked()
{
    QNetworkReply* banReply = MutableTrack( radio->currentTrack() ).ban();
    connect(banReply, SIGNAL(finished()), SLOT(onBanFinished()));
}

void
RadioWidget::onBanFinished()
{
    lastfm::XmlQuery lfm(lastfm::ws::parse(static_cast<QNetworkReply*>(sender())));

    if ( lfm.attribute( "status" ) != "ok" )
    {
    }

    aApp->skipAction()->trigger();
}


void
RadioWidget::onInfoClicked()
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
    LSFindApplicationForInfo( kLSUnknownCreator, CFSTR( "fm.last.scrobbler" ), NULL, &appRef, NULL );
    OSStatus status = LSOpenFSRef( &appRef, NULL );
#endif
}


void
RadioWidget::onFilterClicked()
{
    TagFilterDialog tagFilter( radio->station(), this );
    if ( tagFilter.exec() == QDialog::Accepted )
    {
        RadioStation station = radio->station();
        station.setTagFilter( tagFilter.tag() );
        radio->playNext( station );
    }
}


void
RadioWidget::onEditClicked()
{

}

void
RadioWidget::onRadioTick( qint64 tick )
{
    ui->bar->setValue( ui->bar->maximum() < tick ? ui->bar->maximum() : tick );

    if( tick > 0 )
    {
        QTime time( 0, 0 );
        time = time.addSecs( ( tick / 1000 ) );
        ui->time->setText( time.toString( "mm:ss" ) );
        QTime timeToGo( 0, 0 );
        timeToGo = timeToGo.addSecs( ui->bar->maximum() < tick ? 0 : ( ui->bar->maximum() - tick ) / 1000 );
        ui->timeToGo->setText( timeToGo.toString( "-mm:ss" ) );
    }
}


void
RadioWidget::onTuningIn( const RadioStation& station )
{
    ui->radioTitle->setText( tr("Tuning %1").arg( station.title() ) );
    ui->play->setChecked( true );
    aApp->playAction()->setChecked( true );
    ui->album->setText( radio->currentTrack().isNull() ? "" : tr("from %1").arg( radio->currentTrack().album() ) );
    ui->trackTitle->setText( radio->currentTrack().isNull() ? "" : radio->currentTrack().toString() );

    ui->onTour->hide();

    setWindowTitle( QString( "Last.fm Radio - %1" ).arg( station.title() ) );

    ui->details->setCurrentWidget( ui->detailsPage );
}

QString userLibrary( const QString& user, const QString& artist )
{
    return Label::anchor( QString("http://www.last.fm/user/%1/library/music/%2").arg( user, artist ), user );
}


void
RadioWidget::onTrackSpooled( const Track& track )
{
    if( !track.isNull() && track.source() == Track::LastFmRadio )
    {
        aApp->playAction()->setEnabled( true );
        aApp->loveAction()->setEnabled( true );
        aApp->banAction()->setEnabled( true );
        aApp->skipAction()->setEnabled( true );

        aApp->playAction()->setChecked( true );
        aApp->loveAction()->setChecked( track.isLoved() );

        connect( qApp, SIGNAL(busLovedStateChanged(bool)), ui->love, SLOT(setChecked(bool)) );
        connect( ui->love, SIGNAL(clicked(bool)), qApp, SLOT(sendBusLovedStateChanged(bool)) );
        connect( track.signalProxy(), SIGNAL(loveToggled(bool)), ui->love, SLOT(setChecked(bool)));
        connect( ui->info, SIGNAL(clicked()), SLOT(onInfoClicked()));

        ui->play->setChecked( true );

        ui->onTour->hide();

        ui->trackTitle->setText( QString( "%1 %2 %3" ).arg(
        Label::anchor( track.artist().www().toString(), track.artist().name()),
        QChar( 0x2013 ),
        Label::anchor( track.www().toString(), track.title()) ) );

        ui->album->setText( QString( "from %1" ).arg( Label::anchor( track.album().www().toString(), track.album().title() ) ) );

        ui->radioTitle->setText( radio->station().title() );

        setWindowTitle( QString( "Last.fm Radio - %1 - %2" ).arg( radio->station().title(), track.toString() ) );

        int trackDuration = track.duration();
        ui->bar->setMinimum( 0 );
        ui->bar->setMaximum( trackDuration * 1000 );
        ui->bar->setValue( 0 );

        QTime t( 0, 0 );
        ui->time->setText( t.toString( "mm:ss" ));
        t = t.addSecs( trackDuration );
        ui->timeToGo->setText( t.toString( "-mm:ss" ));
    }
    else
    {
        ui->play->setChecked( false );

        if( !ui->time->text().isEmpty())
            update();

        QTime t( 0, 0 );
        ui->time->setText( t.toString( "mm:ss" ));
        ui->timeToGo->setText( t.toString( "-mm:ss" ));

        ui->trackTitle->clear();
        ui->album->clear();
        ui->context->clear();
        ui->radioTitle->clear();

        setWindowTitle( QString( "Last.fm Radio" ) );
    }

    ui->onTour->hide();
    connect( track.artist().getEvents( 1 ), SIGNAL(finished()), SLOT(onGotEvents()) ) ;

    ui->nowPlaying->show();

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
                case 1: contextString = tr( "because you listen to %1" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ) ); break;
                case 2: contextString = tr( "because you listen to %1 and %2" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ), Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ) ); break;
                case 3: contextString = tr( "because you listen to %1, %2, and %3" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ) , Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ), Label::anchor( Artist( context.values().at(2) ).www().toString(), context.values().at(2) ) ); break;
                case 4: contextString = tr( "because you listen to %1, %2, %3, and %4" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ), Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ), Label::anchor( Artist( context.values().at(2) ).www().toString(), context.values().at(2) ), Label::anchor( Artist( context.values().at(3) ).www().toString(), context.values().at(3) ) ); break;
                case 5: contextString = tr( "because you listen to %1, %2, %3, %4, and %5" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ), Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ), Label::anchor( Artist( context.values().at(2) ).www().toString(), context.values().at(2) ), Label::anchor( Artist( context.values().at(3) ).www().toString(), context.values().at(3) ), Label::anchor( Artist( context.values().at(4) ).www().toString(), context.values().at(4) ) ); break;
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
                case 1: contextString = tr( "from %2%1s library" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ) ); break;
                case 2: contextString = tr( "from %2 and %3%1s libraries" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ), userLibrary( context.values().at(1), track.artist().name() ) ); break;
                case 3: contextString = tr( "from %2, %3, and %4%1s libraries" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ), userLibrary( context.values().at(1), track.artist().name() ), userLibrary( context.values().at(2), track.artist().name() ) ); break;
                case 4: contextString = tr( "from %2, %3, %4, and %5%1s libraries" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ),userLibrary(  context.values().at(1), track.artist().name() ), userLibrary( context.values().at(2), track.artist().name() ), userLibrary( context.values().at(3), track.artist().name() ) ); break;
                case 5: contextString = tr( "from %2, %3, %4, %5, and %6%1s libraries" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ), userLibrary( context.values().at(1), track.artist().name() ), userLibrary( context.values().at(2), track.artist().name() ), userLibrary( context.values().at(3), track.artist().name() ), userLibrary( context.values().at(4), track.artist().name() ) ); break;
                }
            }
            break;
        }

        ui->context->setText( contextString );

#ifdef CLIENT_ROOM_RADIO
        QString strippedContextString = contextString;

        QRegExp re( "<[^>]*>" );

        strippedContextString.replace( re, "" );

        QString ircMessage = QString( "#last.clientradio %1 %2" ).arg( track.toString(), strippedContextString );

        if ( context.values().count() == ( radio->station().url().count( "," ) + 1 ) )
            ircMessage.append( " BINGO!" );

        QTcpSocket socket;
        socket.connectToHost( "localhost", 12345 );
        socket.waitForConnected();
        socket.write( ircMessage.toUtf8() );
        socket.flush();
        socket.close();
#endif
    }
    else
        ui->context->clear();
}



void
RadioWidget::onError( int error, const QVariant& errorText )
{
    ui->radioTitle->setText( errorText.toString() + ": " + QString::number(error) );
}


void
RadioWidget::onStopped()
{
    aApp->playAction()->setChecked( false );

    ui->love->setEnabled( false );
    ui->ban->setEnabled( false );
    ui->skip->setEnabled( false );

    ui->trackTitle->clear();
    ui->album->clear();

    setWindowTitle( "Last.fm Radio" );

    ui->nowPlaying->hide();
    //ui->details->setCurrentWidget( ui->quickstartPage );
}


void
RadioWidget::onSwitch()
{
    emit aboutToHide();
    hide();
}

void
RadioWidget::onGotRecentStations()
{
//    lastfm::XmlQuery lfm = lastfm::XmlQuery( static_cast<QNetworkReply*>( sender() )->readAll() );

//    foreach ( const lastfm::XmlQuery& station , lfm["recentstations"].children("station").mid( 0, 20 ) )
//    {
//        if ( ! (station["url"].text().contains("user/" + lastfm::ws::Username + "/") &&
//             !station["url"].text().contains("/tag/") ) )
//        {
//            RadioStation recentStation( station["url"].text() );
//            recentStation.setTitle( station["name"].text() );
//            ui->stations->addStation( recentStation, tr("Recent") );
//        }
//    }
}

void
RadioWidget::onGotEvents()
{
    XmlQuery lfm = static_cast<QNetworkReply*>(sender())->readAll();

    if ( lfm["events"].children("event").count() > 0
         && lfm["events"].attribute("artist") == radio->currentTrack().artist() )
    {
        ui->onTour->show();
        ui->onTour->setOpenExternalLinks( true );
        ui->onTour->setText( Label::anchor( radio->currentTrack().artist().www().toString(), tr( "ON TOUR" ) ) );
    }
}

