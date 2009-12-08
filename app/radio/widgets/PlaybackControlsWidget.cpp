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

#include "PlaybackControlsWidget.h"

#include "../Radio.h"
#include <lastfm/RadioStation>

#include <QPushButton>
#include <QApplication>
#include <QHBoxLayout>

#include <QSlider>
#include <QToolButton>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>

#include <Phonon/VolumeSlider>

PlaybackControlsWidget::PlaybackControlsWidget( QWidget* parent )
                       :StylableWidget( parent )
{
    QHBoxLayout* h = new QHBoxLayout( this );
    h->setContentsMargins( 12, 0, 12, 0 );
    h->setSpacing( 5 );

    h->addWidget( ui.volume = new QPushButton( tr( "volume" ) ));
    ui.volume->setObjectName( "volume" );

    ui.volumeSlider = new Phonon::VolumeSlider(window());
    ui.volumeSlider->setObjectName("volumeSlider");
    ui.volumeSlider->setOrientation(Qt::Vertical);
    ui.volumeSlider->setFixedWidth( 30 );
    ui.volumeSlider->setContentsMargins( 0, 4, 0, 4 );
    ui.volumeSlider->layout()->setSpacing( 5 );
    ui.volumeSlider->resize(ui.volumeSlider->width(), 120);
    ui.volumeSlider->hide();
    
    h->addWidget( ui.love = new QPushButton( tr( "love" ) ));
    ui.love->setObjectName( "love" );
    
    h->addWidget( ui.ban = new QPushButton( tr( "ban" ) ));
    ui.ban->setObjectName( "ban" );
    
    h->addWidget( ui.play = new QPushButton( tr( "play" ) ));
    ui.play->setObjectName( "play" );
    ui.play->setCheckable( true );
    ui.play->setChecked( false );
    
    h->addWidget( ui.skip = new QPushButton( tr( "skip" ) ));
    ui.skip->setObjectName( "skip" );
    
	connect( radio, SIGNAL(stopped()), SLOT(onRadioStopped()) );
    connect( radio, SIGNAL(tuningIn( const RadioStation&)), SLOT( onRadioTuningIn( const RadioStation&)));
	connect( ui.play, SIGNAL( clicked()), SLOT( onPlayClicked()) );
    connect( ui.skip, SIGNAL( clicked()), radio, SLOT(skip()));
    connect( ui.volume, SIGNAL( clicked()), SLOT(onVolumeClicked()));

    setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
}

void
PlaybackControlsWidget::onVolumeClicked()
{
    if (!ui.volumeSlider->isVisible())
    {
        ui.volumeSlider->setAudioOutput(radio->audioOutput());
        moveVolumeSlider();
        ui.volumeSlider->setWindowFlags( Qt::Popup );
        ui.volumeSlider->raise();
        ui.volumeSlider->show();
    }
}

void
PlaybackControlsWidget::moveVolumeSlider()
{
    int v2 = ui.volumeSlider->width() / 2;
    int b2 = ui.volume->width() / 2;
    ui.volumeSlider->move(mapToGlobal( ui.volume->pos()) - QPoint(v2 - b2, ui.volumeSlider->height()));
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

