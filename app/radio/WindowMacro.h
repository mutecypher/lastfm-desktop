
#ifndef WINDOWMACRO_H
#define WINDOWMACRO_H

#include "../Radio.h"

#define TRACK_SPOOLED() \
if( !track.isNull() && track.source() == Track::LastFmRadio ) \
{ \
    m_actions->m_playAction->setEnabled( true ); \
    m_actions->m_loveAction->setEnabled( true ); \
    m_actions->m_banAction->setEnabled( true ); \
    m_actions->m_skipAction->setEnabled( true ); \
\
    m_actions->m_playAction->setChecked( true ); \
    m_actions->m_loveAction->setChecked( track.isLoved() ); \
\
    connect( qApp, SIGNAL(busLovedStateChanged(bool)), ui->love, SLOT(setChecked(bool)) ); \
    connect( ui->love, SIGNAL(clicked(bool)), qApp, SLOT(sendBusLovedStateChanged(bool)) ); \
    connect( track.signalProxy(), SIGNAL(loveToggled(bool)), ui->love, SLOT(setChecked(bool))); \
\
    ui->play->setChecked( true ); \
    ui->radioTitle->setText( radio->station().title() ); \
    ui->album->setText( tr("from %1").arg( track.album() ) ); \
    ui->trackTitle->setText( track.toString() ); \
\
    ui->bar->setMinimum( 0 ); \
    ui->bar->setMaximum( track.duration() * 1000 ); \
    ui->bar->setValue( 0 ); \
\
    QTime t( 0, 0 ); \
    ui->time->setText( t.toString( "mm:ss" )); \
    t = t.addSecs( track.duration() ); \
    ui->timeToGo->setText( t.toString( "-mm:ss" )); \
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
m_actions->m_loveAction->setEnabled( false ); \
m_actions->m_banAction->setEnabled( false ); \
m_actions->m_skipAction->setEnabled( false ); \
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
m_actions->connectActionChanges( this ); \
\
connect( radio, SIGNAL(trackSpooled(Track)), SLOT(onTrackSpooled(Track)) ); \
connect( radio, SIGNAL(tick(qint64)), SLOT(onRadioTick(qint64))); \
\
ui->volumeSlider->setAudioOutput( radio->audioOutput() ); \
ui->volumeSlider->setMuteVisible( false ); \
menuWidget()->hide();\
\
new QShortcut( QKeySequence( Qt::CTRL + Qt::Key_M ), this, SLOT(onSwitch())); \
new QShortcut( QKeySequence( Qt::Key_Space ), this, SLOT(onSpace()));

#define ON_TUNING_IN() \
ui->radioTitle->setText( tr("Tuning %1").arg( station.title() ) ); \
ui->play->setChecked( true ); \
m_actions->m_playAction->setChecked( true ); \
ui->album->setText( radio->currentTrack().isNull() ? "" : tr("from %1").arg( radio->currentTrack().album() ) ); \
ui->trackTitle->setText( radio->currentTrack().isNull() ? "" : radio->currentTrack().toString() );

#define SPACE() \
m_actions->m_playAction->trigger();

#define PLAY_CLICKED() \
if ( checked ) \
{ \
    if ( radio->state() == Radio::Stopped ) \
        radio->play( RadioStation( "" ) ); \
    else \
        radio->resume(); \
} \
else \
    radio->pause();

#define SKIP_CLICKED() radio->skip();

#define RADIO_TICK() \
ui->bar->setValue( tick ); \
\
if( tick > 0 ) \
{ \
    QTime time( 0, 0 ); \
    time = time.addSecs( ( tick / 1000 ) ); \
    ui->time->setText( time.toString( "mm:ss" ) ); \
    QTime timeToGo( 0, 0 ); \
    timeToGo = timeToGo.addSecs( ( ui->bar->maximum() - tick ) / 1000 ); \
    ui->timeToGo->setText( timeToGo.toString( "-mm:ss" ) ); \
}


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

#define ON_ACTIONS_CHANGED() \
 \
ui->love->setChecked( m_actions->m_loveAction->isChecked() ); \
ui->ban->setChecked( m_actions->m_banAction->isChecked() ); \
ui->play->setChecked( m_actions->m_playAction->isChecked() ); \
ui->skip->setChecked( m_actions->m_skipAction->isChecked() ); \
 \
ui->love->setEnabled( m_actions->m_loveAction->isEnabled() ); \
ui->ban->setEnabled( m_actions->m_banAction->isEnabled() ); \
ui->play->setEnabled( m_actions->m_playAction->isEnabled() ); \
ui->skip->setEnabled( m_actions->m_skipAction->isEnabled() ); \
 \
ui->love->setToolTip( ui->love->isChecked() ? tr("Unlove") : tr("Love") ); \
ui->ban->setToolTip( tr("Ban") ); \
ui->info->setToolTip( tr("Info") ); \
ui->play->setToolTip( ui->play->isChecked() ? tr("Pause") : tr("Play") ); \
ui->skip->setToolTip( tr("Skip") ); \
 \
ui->love->setText( ui->love->isChecked() ? tr("Unlove") : tr("Love") ); \
ui->ban->setText( tr("Ban") ); \
ui->info->setText( tr("Info") ); \
ui->play->setText( ui->play->isChecked() ? tr("Pause") : tr("Play") ); \
ui->skip->setText( tr("Skip") );

#endif // WINDOWMACRO_H
