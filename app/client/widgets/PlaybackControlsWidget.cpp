
#include <QShortcut>

#include "../Application.h"
#include "../Radio.h"

#include "PlaybackControlsWidget.h"
#include "ui_PlaybackControlsWidget.h"

PlaybackControlsWidget::PlaybackControlsWidget(QWidget *parent) :
    StylableWidget(parent),
    ui(new Ui::PlaybackControlsWidget)
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

    connect( radio, SIGNAL(trackSpooled(Track)), SLOT(onTrackSpooled(Track)) );
    //connect( radio, SIGNAL(tick(qint64)), SLOT(onRadioTick(qint64)));
    connect( radio, SIGNAL(stopped()), SLOT(onStopped()));
    connect( radio, SIGNAL(tuningIn(RadioStation)), SLOT(onTuningIn(RadioStation)));
    connect( radio, SIGNAL(error(int,QVariant)), SLOT(onError(int, QVariant)));

    onActionsChanged();

    // if our buttons are pressed we should trigger the actions
    connect( ui->love, SIGNAL(clicked()), aApp->loveAction(), SLOT(trigger()));
    connect( ui->ban, SIGNAL(clicked()), aApp->banAction(), SLOT(trigger()));
    connect( ui->play, SIGNAL(clicked()), aApp->playAction(), SLOT(trigger()));
    connect( ui->skip, SIGNAL(clicked()), aApp->skipAction(), SLOT(trigger()));

    ui->volumeSlider->setAudioOutput( radio->audioOutput() );
    ui->volumeSlider->setMuteVisible( false );

    new QShortcut( QKeySequence( Qt::Key_Space ), this, SLOT(onSpace()));
}


PlaybackControlsWidget::~PlaybackControlsWidget()
{
    delete ui;
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
PlaybackControlsWidget::onPlayTriggered( bool checked )
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
PlaybackControlsWidget::onSkipClicked()
{
    radio->skip();
}


void
PlaybackControlsWidget::onLoveClicked( bool loved )
{
    MutableTrack track( radio->currentTrack() );

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
    QNetworkReply* banReply = MutableTrack( radio->currentTrack() ).ban();
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
PlaybackControlsWidget::onTuningIn( const RadioStation& station )
{
    show();

    ui->title->setText( tr("Tuning %1").arg( station.title() ) );

    ui->play->setChecked( true );
    aApp->playAction()->setChecked( true );
}


void
PlaybackControlsWidget::onTrackSpooled( const Track& track )
{
    if( !track.isNull() && track.source() == Track::LastFmRadio )
    {
        aApp->playAction()->setEnabled( true );
        aApp->loveAction()->setEnabled( true );
        aApp->banAction()->setEnabled( true );
        aApp->skipAction()->setEnabled( true );

        aApp->playAction()->setChecked( true );
        aApp->loveAction()->setChecked( track.isLoved() );

        connect( track.signalProxy(), SIGNAL(loveToggled(bool)), ui->love, SLOT(setChecked(bool)));

        ui->play->setChecked( true );

        ui->title->setText( radio->station().title() );
    }
    else
    {
        ui->play->setChecked( false );

        ui->title->clear();
    }
}

void
PlaybackControlsWidget::onError( int error, const QVariant& errorText )
{
    ui->title->setText( errorText.toString() + ": " + QString::number(error) );
}

void
PlaybackControlsWidget::onStopped()
{
    aApp->playAction()->setChecked( false );

    ui->love->setEnabled( false );
    ui->ban->setEnabled( false );
    ui->skip->setEnabled( false );
}

