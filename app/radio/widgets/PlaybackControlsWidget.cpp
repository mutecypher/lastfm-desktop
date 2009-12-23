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

#include <lastfm/RadioStation>

#include "PlaybackControlsWidget.h"
#include "AdvancedOptionsDialog.h"
#include "../Radio.h"
#include "VolumeButton.h"

PlaybackControlsWidget::PlaybackControlsWidget( QWidget* parent )
                       :StylableWidget( parent )
{
    QHBoxLayout* everythingLayout = new QHBoxLayout( this );
    setLayout(everythingLayout);

    // set up the popup buttons frame
    QFrame* popupsFrame = new QFrame( this );
    popupsFrame->setObjectName( "popupsFrame" );
    popupsFrame->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    everythingLayout->addWidget(popupsFrame, 0, Qt::AlignLeft);
    QHBoxLayout* popupsLayout = new QHBoxLayout( popupsFrame );
    popupsFrame->setLayout(popupsLayout);
    popupsLayout->setContentsMargins( 5, 5, 5, 5 );
    popupsLayout->setSpacing( 5 );

    popupsLayout->addWidget( ui.volume = new VolumeButton( tr( "volume" ) ));
    ui.volume->setObjectName( "volume" );

    popupsLayout->addWidget( ui.options = new QPushButton( tr( "options" ) ));
    ui.options->setObjectName( "options" );
    ui.optionsDialog = 0;

    everythingLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    // add the control buttons
    QFrame* controlsFrame = new QFrame(this);
    controlsFrame->setObjectName( "controlsFrame" );
    controlsFrame->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    everythingLayout->addWidget(controlsFrame, 0, Qt::AlignCenter | Qt::AlignAbsolute);
    QHBoxLayout* controlsLayout = new QHBoxLayout(this);
    controlsFrame->setLayout(controlsLayout);
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

    everythingLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    everythingLayout->addSpacerItem(new QSpacerItem(popupsFrame->width(), 0, QSizePolicy::Fixed, QSizePolicy::Expanding));

	connect( radio, SIGNAL(stopped()), SLOT(onRadioStopped()) );
    connect( radio, SIGNAL(tuningIn( const RadioStation&)), SLOT( onRadioTuningIn( const RadioStation&)));
	connect( ui.play, SIGNAL( clicked()), SLOT( onPlayClicked()) );
    connect( ui.skip, SIGNAL( clicked()), radio, SLOT(skip()));
    connect( ui.options, SIGNAL( clicked()), SLOT(onOptionsClicked()));

    setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
}

void
PlaybackControlsWidget::onOptionsClicked()
{
    if (!ui.optionsDialog)
    {
        // show the options widget as a window
        ui.optionsDialog = new AdvancedOptionsDialog();

        // add an ok/cancel button box to the control
        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        ui.optionsDialog->layout()->addWidget(buttonBox);

        connect(buttonBox, SIGNAL(accepted()), ui.optionsDialog, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), ui.optionsDialog, SLOT(reject()));
        connect(ui.optionsDialog, SIGNAL(finished(int)), SLOT(onOptionsFinished(int)));

        ui.optionsDialog->show();
    }
}

void
PlaybackControlsWidget::onOptionsFinished(int result)
{
    qDebug() << "finished" << result;

    ui.optionsDialog = 0;

    if (result == QDialogButtonBox::Ok)
    {
        // they clicked OK, so lets do what they asked for

        // do not stop the radio but make sure the next track
        // is of the new radio settings type
    }
}

void
PlaybackControlsWidget::onRadioStopped()
{
    ui.play->setChecked( false );
}


void 
PlaybackControlsWidget::onRadioTuningIn( const RadioStation& )
{
    ui.play->setChecked( true );
}


void
PlaybackControlsWidget::onPlayClicked()
{
	if (!ui.play->isChecked())
		radio->stop();
    else
        radio->play( RadioStation( "" ) );
}
