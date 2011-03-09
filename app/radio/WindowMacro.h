
#ifndef WINDOWMACRO_H
#define WINDOWMACRO_H

#include "../Radio.h"

#define TRACK_SPOOLED() \
if( !track.isNull() && track.source() == Track::LastFmRadio ) \
{ \
    ui->play->setEnabled( true ); \
    ui->love->setEnabled( true ); \
    ui->ban->setEnabled( true ); \
    ui->info->setEnabled( true ); \
\
    ui->love->setEnabled( true ); \
    ui->ban->setEnabled( true ); \
    ui->info->setEnabled( true ); \
\
    ui->play->setChecked( true ); \
    m_actions->m_loveAction->setChecked( track.isLoved() ); \
\
    connect( qApp, SIGNAL(busLovedStateChanged(bool)), ui->love, SLOT(setChecked(bool)) ); \
    connect( ui->love, SIGNAL(clicked(bool)), qApp, SLOT(sendBusLovedStateChanged(bool)) ); \
    connect( track.signalProxy(), SIGNAL(loveToggled(bool)), ui->love, SLOT(setChecked(bool))); \
\
    ui->play->setChecked( true ); \
    ui->radioTitle->setText( radio->station().title() ); \
    ui->trackTitle->setText( track.toString() ); \
\
    ui->bar->setMinimum( 0 ); \
    ui->bar->setMaximum( track.duration() ); \
    ui->bar->setValue( 0 ); \
\
    QTime t( 0, 0 ); \
    ui->time->setText( t.toString( "mm:ss" )); \
    t = t.addSecs( track.duration() ); \
    ui->timeToGo->setText( t.toString( "mm:ss" )); \
    ui->time->setVisible( true ); \
    ui->timeToGo->setVisible( true ); \
} \
else \
{ \
    ui->play->setChecked( false ); \
\
    if( !ui->time->text().isEmpty()) \
        update(); \
\
    ui->time->setHidden( true ); \
    ui->timeToGo->setHidden( true ); \
}

#define SETUP() \
ui->setupUi(this); \
finishUi(); \
\
connect( ui->love, SIGNAL(clicked()), m_actions->m_loveAction, SLOT(trigger())); \
connect( ui->ban, SIGNAL(clicked()), m_actions->m_banAction, SLOT(trigger())); \
connect( ui->play, SIGNAL(clicked()), m_actions->m_playAction, SLOT(trigger())); \
connect( ui->skip, SIGNAL(clicked()), m_actions->m_skipAction, SLOT(trigger())); \
connect( m_actions->m_loveAction, SIGNAL(changed()), SLOT(onActionsChanged())); \
connect( m_actions->m_playAction, SIGNAL(changed()), SLOT(onActionsChanged())); \
connect( m_actions->m_skipAction, SIGNAL(changed()), SLOT(onActionsChanged())); \
connect( m_actions->m_banAction, SIGNAL(changed()), SLOT(onActionsChanged())); \
\
m_actions->doConnect( this ); \
\
connect( radio, SIGNAL(trackSpooled(Track)), SLOT(onTrackSpooled(Track)) ); \
connect( radio, SIGNAL(tick(qint64)), SLOT(onRadioTick(qint64))); \
\
ui->volumeSlider->setAudioOutput( radio->audioOutput() );

#define ON_TUNING_IN() \
ui->radioTitle->setText( tr("Tuning %1").arg( station.title() ) ); \
ui->play->setChecked( true ); \
m_actions->m_playAction->setChecked( true );

#define PLAY_CLICKED() \
if ( checked ) \
    radio->resume(); \
else \
    radio->pause();

#define SKIP_CLICKED() radio->skip();

#define RADIO_TICK() \
ui->bar->setValue( tick / 1000 ); \
\
QTime t( 0, 0 ); \
t = t.addSecs( tick / 1000 ); \
if( tick > 0 ) \
    ui->time->setText( t.toString( "mm:ss" ));

#define LOVE_CLICKED() \
MutableTrack track( radio->currentTrack() ); \
\
if ( loved ) \
    track.love(); \
else \
    track.unlove(); \
\
connect( track.signalProxy(), SIGNAL(loveToggled(bool)), ui->love, SLOT(setChecked(bool)));

#define LOVE_TRIGGERED() \
ui->love->setChecked( !ui->love->isChecked() ); \
onLoveClicked( ui->love->isChecked() );

#define BAN_CLICKED() \
QNetworkReply* banReply = MutableTrack( radio->currentTrack() ).ban(); \
connect(banReply, SIGNAL(finished()), SLOT(onBanFinished()));

#define BAN_FINISHED() \
\
lastfm::XmlQuery lfm(lastfm::ws::parse(static_cast<QNetworkReply*>(sender()))); \
 \
if ( lfm.attribute( "status" ) != "ok" ) \
{ \
} \
\
m_actions->m_skipAction->trigger();

#define ON_FILTER_CLICKED() \
TagFilterDialog tagFilter( radio->station(), this ); \
if ( tagFilter.exec() == QDialog::Accepted ) \
{ \
    RadioStation station = radio->station(); \
    station.setTagFilter( tagFilter.tag() ); \
    radio->playNext( station ); \
}

#define ON_EDIT_CLICKED()

#endif // WINDOWMACRO_H
