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

#include <Phonon/VolumeSlider>

#include <lastfm/RadioStation>

#include "PlaybackControlsWidget.h"
#include "AdvancedOptionsWidget.h"
#include "AdvancedOptionsDialog.h"
#include "../Radio.h"

PlaybackControlsWidget::PlaybackControlsWidget( QWidget* parent )
                       :StylableWidget( parent )
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    setLayout(layout);

    layout->addWidget( ui.radioOptions = new QPushButton( tr( "radio options" ) ));
    ui.radioOptions->setObjectName( "radioOptions" );

    {
        QHBoxLayout* volumeLayout = new QHBoxLayout( this );
        volumeLayout->setObjectName( "volumeLayout" );
        volumeLayout->setContentsMargins( 0, 0, 0, 0 );
        volumeLayout->setSpacing( 0 );

        volumeLayout->setSizeConstraint( QLayout::SetMinimumSize );

        QLabel* volumeLeft = new QLabel( "volume left", this );
        volumeLeft->setObjectName( "volumeLeft" );
        volumeLayout->addWidget( volumeLeft );

        // add the volume slider on the left
        volumeLayout->addWidget( ui.volume = new Phonon::VolumeSlider(radio->audioOutput()));

        QLabel* volumeRight = new QLabel( "volume right", this );
        volumeRight->setObjectName( "volumeRight" );
        volumeLayout->addWidget( volumeRight );

        ui.volume->setMuteVisible( false );
        ui.volume->setOrientation( Qt::Horizontal );
        ui.volume->setObjectName( "volume" );

        layout->addLayout( volumeLayout );
    }

    layout->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    {
        // add the control buttons
        QHBoxLayout* controlsLayout = new QHBoxLayout( this );
        controlsLayout->setObjectName( "controls" );
        controlsLayout->setContentsMargins( 5, 5, 5, 5 );
        controlsLayout->setSpacing( 5 );

        controlsLayout->addWidget( ui.love = new QPushButton( tr( "love" ) ));
        ui.love->setObjectName( "love" );

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

    layout->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    layout->addWidget( ui.cog = new QToolButton( this ));
    ui.cog->setObjectName( "cog" );
    ui.cog->setAutoRaise( true );
    ui.cog->setPopupMode( QToolButton::InstantPopup );

    QMenu* cogMenu = new QMenu(this);
    //cogMenu->addAction("Radio options", this, SLOT(onRadioOptionsClicked()))->setObjectName("radioOptions");
    cogMenu->addAction("Tag", this, SLOT(onRadioOptionsClicked()))->setObjectName("tag");
    cogMenu->addAction("Share", this, SLOT(onRadioOptionsClicked()))->setObjectName("share");
    ui.cog->setMenu(cogMenu);

    ui.radioOptionsDialog = 0;

	connect( radio, SIGNAL(stopped()), SLOT(onRadioStopped()) );
    connect( radio, SIGNAL(tuningIn( const RadioStation&)), SLOT( onRadioTuningIn( const RadioStation&)));
    connect( ui.play, SIGNAL( toggled(bool)), SLOT( onPlayToggled(bool)) );
    connect( ui.skip, SIGNAL( clicked()), radio, SLOT(skip()));
    connect( ui.radioOptions, SIGNAL( clicked()), SLOT(onRadioOptionsClicked()));

    setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

    // make the space button check and uncheck the play button
    new QShortcut( QKeySequence(Qt::Key_Space), ui.play, SLOT(toggle()) );

    setButtonsEnabled( false );
}

void
PlaybackControlsWidget::onRadioOptionsClicked()
{
    // show the menu

    if ( !ui.radioOptionsDialog )
    {
        // show the options widget as a window
        ui.radioOptionsDialog = new AdvancedOptionsDialog();

        // set the options in the widget to that of the current radio station
        ui.radioOptionsDialog->widget().setRep( radio->station().rep() );
        ui.radioOptionsDialog->widget().setMainstr( radio->station().mainstr() );
        ui.radioOptionsDialog->widget().setDisco( radio->station().disco() );

        // add an ok/cancel button box to the control
        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        ui.radioOptionsDialog->layout()->addWidget(buttonBox);

        // connect some signals so we knoe when the dialog closes
        connect(buttonBox, SIGNAL(accepted()), ui.radioOptionsDialog, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), ui.radioOptionsDialog, SLOT(reject()));
        connect(ui.radioOptionsDialog, SIGNAL(finished(int)), SLOT(onRadioOptionsFinished(int)));

        ui.radioOptionsDialog->show();
    }
}

void
PlaybackControlsWidget::onRadioOptionsFinished(int result)
{
    if (result == QDialog::Accepted)
    {
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

    ui.radioOptionsDialog = 0;
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
PlaybackControlsWidget::setButtonsEnabled( bool enabled )
{
    ui.skip->setEnabled( enabled );
    ui.love->setEnabled( enabled );
    ui.ban->setEnabled( enabled );
}

void
PlaybackControlsWidget::onPlayToggled( bool checked )
{
    if ( !checked )
		radio->stop();
    else
    {
        radio->play( RadioStation( "" ) );
        emit startRadio( RadioStation( "" ) );
    }
}
