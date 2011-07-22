
#include <QShortcut>

#include "../Application.h"
#include "../Services/RadioService.h"
#include "../Services/ScrobbleService.h"

#include "PlaybackControlsWidget.h"
#include "ui_PlaybackControlsWidget.h"

PlaybackControlsWidget::PlaybackControlsWidget(QWidget *parent) :
    StylableWidget(parent),
    ui(new Ui::PlaybackControlsWidget),
    m_scrobbleTrack( false )
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

    //connect( &RadioService::instance(), SIGNAL(trackSpooled(Track)), SLOT(onTrackSpooled(Track)) );
    //connect( &RadioService::instance(), SIGNAL(tick(qint64)), SLOT(onRadioTick(qint64)));
    //connect( &RadioService::instance(), SIGNAL(stopped()), SLOT(onStopped()));
    connect( &RadioService::instance(), SIGNAL(tuningIn(RadioStation)), SLOT(onTuningIn(RadioStation)));
    connect( &RadioService::instance(), SIGNAL(error(int,QVariant)), SLOT(onError(int, QVariant)));

    connect( &ScrobbleService::instance(), SIGNAL(trackStarted(Track,Track)), SLOT(onTrackStarted(Track,Track)) );
    connect( &ScrobbleService::instance(), SIGNAL(stopped()), SLOT(onStopped()));

    onActionsChanged();

    // if our buttons are pressed we should trigger the actions
    connect( ui->love, SIGNAL(clicked()), aApp->loveAction(), SLOT(trigger()));
    connect( ui->ban, SIGNAL(clicked()), aApp->banAction(), SLOT(trigger()));
    connect( ui->play, SIGNAL(clicked()), aApp->playAction(), SLOT(trigger()));
    connect( ui->skip, SIGNAL(clicked()), aApp->skipAction(), SLOT(trigger()));

//    ui->volumeSlider->setAudioOutput( RadioService::instance().audioOutput() );
//    ui->volumeSlider->setMuteVisible( false );

    new QShortcut( QKeySequence( Qt::Key_Space ), this, SLOT(onSpace()));
}


PlaybackControlsWidget::~PlaybackControlsWidget()
{
    delete ui;
}


void
PlaybackControlsWidget::setScrobbleTrack( bool scrobbleTrack )
{
    m_scrobbleTrack = scrobbleTrack;
    style()->polish( this );
    style()->polish( ui->details );
    style()->polish( ui->status );
    style()->polish( ui->device );
}

void
PlaybackControlsWidget::onActionsChanged()
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
   ui->play->setToolTip( ui->play->isChecked() ? tr("Pause") : tr("Play") );
   ui->skip->setToolTip( tr("Skip") );

   ui->love->setText( ui->love->isChecked() ? tr("Unlove") : tr("Love") );
   ui->ban->setText( tr("Ban") );
   ui->play->setText( ui->play->isChecked() ? tr("Pause") : tr("Play") );
   ui->skip->setText( tr("Skip") );
}


void
PlaybackControlsWidget::onSpace()
{
    aApp->playAction()->trigger();
}


void
PlaybackControlsWidget::onPlayClicked( bool checked )
{
    if ( checked )
    {
        if ( RadioService::instance().state() == Stopped )
            RadioService::instance().play( RadioStation( "" ) );
        else
        {
            RadioService::instance().resume();
        }
    }
    else
    {
        RadioService::instance().pause();
    }
}


void
PlaybackControlsWidget::onPlayTriggered( bool checked )
{
    if ( checked )
    {
        if ( RadioService::instance().state() != Stopped )
            setWindowTitle( QString( "Last.fm Radio - %1 - %2" ).arg( RadioService::instance().station().title(), RadioService::instance().currentTrack().toString() ) );
    }
    else
        setWindowTitle( QString( "Last.fm Radio - %1" ).arg( RadioService::instance().station().title() ) );
}


void
PlaybackControlsWidget::onSkipClicked()
{
    RadioService::instance().skip();
}


void
PlaybackControlsWidget::onLoveClicked( bool loved )
{
    MutableTrack track( RadioService::instance().currentTrack() );

    if ( loved )
        track.love();
    else
        track.unlove();

    connect( track.signalProxy(), SIGNAL(loveToggled(bool)), ui->love, SLOT(setChecked(bool)));
}


void
PlaybackControlsWidget::onLoveTriggered( bool loved )
{
    ui->love->setChecked( loved );
    onLoveClicked( loved );
}


void
PlaybackControlsWidget::onBanClicked()
{
    QNetworkReply* banReply = MutableTrack( RadioService::instance().currentTrack() ).ban();
    connect(banReply, SIGNAL(finished()), SLOT(onBanFinished()));
}


void
PlaybackControlsWidget::onBanFinished()
{
    lastfm::XmlQuery lfm(lastfm::ws::parse(static_cast<QNetworkReply*>(sender())));

    if ( lfm.attribute( "status" ) != "ok" )
    {
    }

    aApp->skipAction()->trigger();
}

void
PlaybackControlsWidget::setIconForRadio( const RadioStation& station )
{
    QString url = station.url();

    if ( url.startsWith("lastfm://user") )
    {
        if ( url.contains( "/friends" )
            || url.contains( "/neighbours" )
            || url.startsWith( "lastfm://users") )
            ui->icon->setPixmap( QPixmap( ":/control_bar_radio_friends.png" ) );
        if ( url.contains( "/library" )
             || url.contains( "/personal") )
            ui->icon->setPixmap( QPixmap( ":/control_bar_radio_library.png" ) );
        else if ( url.contains( "/mix" ) )
            ui->icon->setPixmap( QPixmap( ":/control_bar_radio_mix.png" ) );
        else if ( url.contains( "/recommended" ) )
            ui->icon->setPixmap( QPixmap( ":/control_bar_radio_rec.png" ) );

    }
    else if ( url.startsWith("lastfm://artist") )
        ui->icon->setPixmap( QPixmap( ":/control_bar_radio_artist.png" ) );
    else if ( url.startsWith("lastfm://tag")
                || url.startsWith( "lastfm://globaltags" ) )
        ui->icon->setPixmap( QPixmap( ":/control_bar_radio_tag.png" ) );

    //ui->icon->setPixmap( ":/control_bar_back.png" );
}

void
PlaybackControlsWidget::onTuningIn( const RadioStation& station )
{
    setScrobbleTrack( false );

    ui->status->setText( tr("Tuning...") );
    ui->device->setText( station.title() );

    ui->play->setChecked( true );
    aApp->playAction()->setChecked( true );

    setIconForRadio( station );

    ui->progressBar->setRange( 0, 1000 );
    ui->progressBar->setValue( 0 );
}

void
PlaybackControlsWidget::onTrackStarted( const Track& track, const Track& oldTrack )
{
    if ( !track.isNull() )
    {
        ui->progressBar->setRange( 0, track.duration() * 1000 );
        ui->progressBar->setValue( 0 );

        disconnect( 0, 0, this, SLOT(onTick(qint64)));
        disconnect( 0, 0, ui->progressBar, SLOT(setValue(int)) );

        if(  track.source() == Track::LastFmRadio )
        {
            setScrobbleTrack( false );

            // A radio track!
            aApp->playAction()->setEnabled( true );
            aApp->loveAction()->setEnabled( true );
            aApp->banAction()->setEnabled( true );
            aApp->skipAction()->setEnabled( true );

            aApp->playAction()->setChecked( true );
            aApp->loveAction()->setChecked( track.isLoved() );

            connect( track.signalProxy(), SIGNAL(loveToggled(bool)), ui->love, SLOT(setChecked(bool)));

            ui->play->setChecked( true );

            ui->controls->show();

            ui->status->setText( tr("Listening to...") );
            ui->device->setText( RadioService::instance().station().title() );

            connect( &RadioService::instance(), SIGNAL(tick(qint64)), SLOT(onTick(qint64)) );
        }
        else
        {
            setScrobbleTrack( true );

            aApp->playAction()->setEnabled( false );
            aApp->loveAction()->setEnabled( true );
            aApp->banAction()->setEnabled( false );
            aApp->skipAction()->setEnabled( false );

            ui->controls->hide();

            // Not a radio track so hide the playback controls!
            ui->status->setText( tr("Scrobbling from...") );
            ui->device->setText( track.extra( "playerName" ) );

            connect( &ScrobbleService::instance(), SIGNAL(frameChanged(int)), ui->progressBar, SLOT(setValue(int)) );
        }

        // Set the icon!
        QString id = track.extra( "playerId" );

        if ( id == "osx" || id == "itw" )
            ui->icon->setPixmap( QPixmap( ":/control_bar_scrobble_itunes.png" ) );
        else if (id == "foo")
            ui->icon->setPixmap( QPixmap( ":/control_bar_scrobble_foobar.png" ) );
        else if (id == "wa2")
            ui->icon->setPixmap( QPixmap( ":/control_bar_scrobble_winamp.png" ) );
        else if (id == "wmp")
            ui->icon->setPixmap( QPixmap( ":/control_bar_scrobble_wmp.png" ) );
        else if (id == "ass")
            setIconForRadio( RadioService::instance().station() );
    }
}

void
PlaybackControlsWidget::onTick( qint64 tick )
{
    ui->progressBar->setValue( tick );
}

void
PlaybackControlsWidget::onError( int error, const QVariant& errorText )
{
    ui->status->setText( errorText.toString() + ": " + QString::number(error) );
}

void
PlaybackControlsWidget::onStopped()
{
    aApp->playAction()->setChecked( false );

    ui->love->setEnabled( false );
    ui->ban->setEnabled( false );
    ui->skip->setEnabled( false );
}

