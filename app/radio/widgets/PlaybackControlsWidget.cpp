/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QPushButton>
#include <QApplication>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QNetworkReply>
#include <QProcess>

#include <QSlider>
#include <QToolButton>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QMdiSubWindow>
#include <QDialogButtonBox>
#include <QShortcut>
#include <QMenu>
#include <QLabel>

#include <phonon/volumeslider.h>

#include <lastfm/RadioStation>

#include "PlaybackControlsWidget.h"
#include "AdvancedOptionsWidget.h"
#include "AdvancedOptionsDialog.h"
#include "../Radio.h"

#include <lastfm/XmlQuery>
#include <lastfm/Track>

#include "lib/unicorn/widgets/TagDialog.h"
#include "lib/unicorn/widgets/ShareDialog.h"

#include "lib/unicorn/UnicornSettings.h"

#ifdef Q_OS_WIN32
#include "windows.h"
#endif

PlaybackControlsWidget::PlaybackControlsWidget( QWidget* parent )
                       :StylableWidget( parent )
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    setLayout(layout);

    layout->addWidget( ui.radioOptions = new QPushButton( tr( "radio options" ) ));
    ui.radioOptions->setObjectName( "radioOptions" );
    ui.radioOptions->setCheckable( true );

    ui.radioOptionsDialog = new AdvancedOptionsDialog( this );

    connect( radio, SIGNAL(supportsDisco( bool )), &ui.radioOptionsDialog->widget(), SLOT(setSupportsDisco( bool )) );

    restoreRadioOptions();
    setRadioOptionsChecked();

    // add an ok/cancel button box to the control
    QDialogButtonBox* buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    ui.radioOptionsDialog->layout()->addWidget( buttonBox );

    // connect some signals so we know when the dialog closes
    connect(buttonBox, SIGNAL(accepted()), ui.radioOptionsDialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), ui.radioOptionsDialog, SLOT(reject()));
    connect(ui.radioOptionsDialog, SIGNAL(finished(int)), SLOT(onRadioOptionsFinished(int)));

    {
        QHBoxLayout* volumeLayout = new QHBoxLayout( this );
        volumeLayout->setObjectName( "volumeLayout" );

        QLabel* volumeLeft = new QLabel( "Volume Down", this );
        volumeLeft->setObjectName( "volumeLeft" );
        volumeLayout->addWidget( volumeLeft );

        // add the volume slider on the left
        volumeLayout->addWidget( ui.volume = new Phonon::VolumeSlider(radio->audioOutput()));

        QLabel* volumeRight = new QLabel( "Volume Up", this );
        volumeRight->setObjectName( "volumeRight" );
        volumeLayout->addWidget( volumeRight );

        ui.volume->setMuteVisible( false );
        ui.volume->setOrientation( Qt::Horizontal );
        ui.volume->setObjectName( "volume" );
        volumeLayout->setContentsMargins( 0, 0, 0, 0 );
        volumeLayout->setSpacing( 0 );


        layout->addLayout( volumeLayout );
    }

    layout->addSpacing( 20 );
    layout->addStretch( 20 );

    {
        // add the control buttons
        QHBoxLayout* controlsLayout = new QHBoxLayout( this );
        controlsLayout->setObjectName( "controls" );
        controlsLayout->setContentsMargins( 0, 0, 0, 0 );
        controlsLayout->setSpacing( 0 );

        controlsLayout->addWidget( ui.love = new QPushButton( tr( "love" ) ));
        ui.love->setObjectName( "love" );
        ui.love->setCheckable( true );

        controlsLayout->addWidget( ui.ban = new QPushButton( tr( "ban" ) ));
        ui.ban->setObjectName( "ban" );

        controlsLayout->addWidget( ui.play = new QPushButton( tr( "play" ) ));
        ui.play->setObjectName( "play" );
        ui.play->setCheckable( true );
        ui.play->setChecked( false );

        controlsLayout->addWidget( ui.skip = new QPushButton( tr( "skip" ) ));
        ui.skip->setObjectName( "skip" );

        layout->addLayout( controlsLayout );
    }

    layout->addSpacing( 20 );
    layout->addStretch( 20 );

    layout->addWidget( ui.info = new QPushButton( tr( "info" ) ));
    ui.info->setObjectName( "info" );

    layout->addWidget( ui.cog = new QToolButton( this ));
    ui.cog->setObjectName( "cog" );
    ui.cog->setAutoRaise( true );
    ui.cog->setPopupMode( QToolButton::InstantPopup );

    QMenu* cogMenu = new QMenu(this);
    ui.tagAction = cogMenu->addAction(QIcon(":/tag-small.png"), "Tag", this, SLOT(onTagClicked()));
    ui.tagAction->setObjectName("tag");
    ui.shareAction = cogMenu->addAction(QIcon(":/share-small.png"), "Share", this, SLOT(onShareClicked()));
    ui.shareAction->setObjectName("share");
    ui.cog->setMenu(cogMenu);

	connect( radio, SIGNAL(stopped()), SLOT(onRadioStopped()) );
    connect( radio, SIGNAL(tuningIn( const RadioStation&)), SLOT( onRadioTuningIn( const RadioStation&)));
    connect( radio, SIGNAL(trackSpooled(Track)), SLOT(onTrackSpooled(Track)));

    connect( ui.love, SIGNAL(clicked(bool)), SLOT(onLoveClicked(bool)));
    connect( ui.ban, SIGNAL(clicked()), SLOT(onBanClicked()));
    connect( ui.play, SIGNAL( clicked(bool)), SLOT( onPlayClicked(bool)) );
    connect( ui.skip, SIGNAL( clicked()), radio, SLOT(skip()));
    connect( ui.info, SIGNAL( clicked()), SLOT(onInfoClicked()));
    connect( ui.radioOptions, SIGNAL( clicked(bool)), SLOT(onRadioOptionsClicked(bool)));

    setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

    // make the space button check and uncheck the play button
    new QShortcut( QKeySequence(Qt::Key_Space), this, SLOT(onSpaceKey()) );

    setButtonsEnabled( false );
}

void
PlaybackControlsWidget::onRadioOptionsClicked( bool /*checked*/ )
{
    // set the options in the widget to that of the current radio station
    ui.radioOptionsDialog->show();

    setRadioOptionsChecked();
}

void
PlaybackControlsWidget::onRadioOptionsFinished(int result)
{
    if (result == QDialog::Accepted)
    {
        saveRadioOptions();

        // if any of the options have been edited, change the radio station
        // so that the next track is of the new station options

        lastfm::RadioStation station = radio->station();

        bool optionsChanged(false);

        if ( ui.radioOptionsDialog->widget().rep() != station.rep() )
        {
            optionsChanged = true;
            station.setRep( ui.radioOptionsDialog->widget().rep() );
        }

        if ( ui.radioOptionsDialog->widget().mainstr() != station.mainstr() )
        {
            optionsChanged = true;
            station.setMainstr( ui.radioOptionsDialog->widget().mainstr() );
        }

        if ( ui.radioOptionsDialog->widget().disco() != station.disco() )
        {
            optionsChanged = true;
            station.setDisco( ui.radioOptionsDialog->widget().disco() );
        }

        if ( optionsChanged )
        {
            radio->playNext(station);
        }
    }
    else
    {
        restoreRadioOptions();
    }

    setRadioOptionsChecked();
}

void
PlaybackControlsWidget::onRadioStopped()
{
    // make sure the play/stop button is in the correct state
    ui.play->setChecked( false );

    // when the radio is stopped we can only use the play and volue controls
    setButtonsEnabled( false );
}

void 
PlaybackControlsWidget::onRadioTuningIn( const RadioStation& )
{
    // make sure the play/stop button is in the correct state
    ui.play->setChecked( true );

    // when the radio is playing we can use all the controls
    setButtonsEnabled( true );
}

void
PlaybackControlsWidget::onTrackSpooled( const Track& track )
{
    ui.love->setChecked( track.isLoved() );
}

void
PlaybackControlsWidget::saveRadioOptions()
{
    unicorn::AppSettings().setValue( "rep", ui.radioOptionsDialog->widget().rep() );
    unicorn::AppSettings().setValue( "mainstr", ui.radioOptionsDialog->widget().mainstr() );
    unicorn::AppSettings().setValue( "disco", ui.radioOptionsDialog->widget().disco() );
}

void
PlaybackControlsWidget::restoreRadioOptions()
{
    bool ok;
    ui.radioOptionsDialog->widget().setRep( unicorn::AppSettings().value( "rep", 0.5 ).toDouble( &ok ) );
    ui.radioOptionsDialog->widget().setMainstr( unicorn::AppSettings().value( "mainstr", 0.5 ).toDouble( &ok ) );
    ui.radioOptionsDialog->widget().setDisco( unicorn::AppSettings().value( "disco", false ).toBool() );
}

void
PlaybackControlsWidget::setRadioOptionsChecked()
{
    ui.radioOptions->setChecked( ui.radioOptionsDialog->widget().rep() != 0.5
                                || ui.radioOptionsDialog->widget().mainstr() != 0.5
                                || ui.radioOptionsDialog->widget().disco() != false );
}

void
PlaybackControlsWidget::setButtonsEnabled( bool enabled )
{
    ui.skip->setEnabled( enabled );
    ui.love->setEnabled( enabled );
    ui.love->setChecked( false );
    ui.ban->setEnabled( enabled );
    ui.info->setEnabled( enabled );
    ui.tagAction->setEnabled( enabled );
    ui.shareAction->setEnabled( enabled );
}

void
PlaybackControlsWidget::onLoveClicked(bool checked)
{
    if ( checked )
    {
        // The button has been checked so love the track!
        QNetworkReply* loveReply = MutableTrack( radio->currentTrack() ).love();
        connect(loveReply, SIGNAL(finished()), SLOT(onLoveFinished()));
    }
    else
    {
        // The button has been unchecked so unlove the track
    }
}

void
PlaybackControlsWidget::onLoveFinished()
{
    // check that the command was succesful
    lastfm::XmlQuery lfm(lastfm::ws::parse(static_cast<QNetworkReply*>(sender())));

    if ( lfm.attribute( "status" ) == "ok" )
    {
        // TODO: show some feedback
    }
    else
    {
        // TODO: error!
    }
}

void
PlaybackControlsWidget::onBanClicked()
{
    // Ban the track
    QNetworkReply* banReply = MutableTrack( radio->currentTrack() ).ban();
    connect(banReply, SIGNAL(finished()), SLOT(onBanFinished()));
}

void
PlaybackControlsWidget::onBanFinished()
{
    // ban the track and skip to the next track (without scrobbling!)
    lastfm::XmlQuery lfm(lastfm::ws::parse(static_cast<QNetworkReply*>(sender())));

    if ( lfm.attribute( "status" ) == "ok" )
    {
        // skip to the next track without scrobbling
        radio->skip();
    }
    else
    {
        // TODO: error!
    }
}

void
PlaybackControlsWidget::onPlayClicked( bool checked )
{
    if ( !checked )
		radio->stop();
    else
    {
        radio->play( RadioStation( "" ) );
        emit startRadio( RadioStation( "" ) );
    }
}

void
PlaybackControlsWidget::onSpaceKey()
{
    if ( !ui.play->isChecked() )
    {
        radio->play( RadioStation( "" ) );
        emit startRadio( RadioStation( "" ) );
    }
    else
        radio->stop();
}

void
PlaybackControlsWidget::onInfoClicked()
{
    QTimer::singleShot(1000, this, SLOT(openScrobbler()));
#ifdef Q_OS_WIN32
    AllowSetForegroundWindow(ASFW_ANY);
#endif
}

void
PlaybackControlsWidget::openScrobbler()
{
    QProcess* process = new QProcess;
    process->start( qApp->applicationDirPath() + "\\audioscrobbler.exe" );
}

void
PlaybackControlsWidget::onTagClicked()
{
    // open the tag dialog
    TagDialog* td = new TagDialog( radio->currentTrack(), window() );
    td->show();
}

void
PlaybackControlsWidget::onShareClicked()
{
    // open the share dialog
    ShareDialog* sd = new ShareDialog( radio->currentTrack(), window() );
    sd->show();
}
