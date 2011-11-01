/*
   Copyright 2010 Last.fm Ltd.
      - Primarily authored by Jono Cole, Michael Coffey, and William Viana

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ui_ScrobbleSettingsWidget.h"
#include "ScrobbleSettingsWidget.h"

#include "lib/unicorn/UnicornSettings.h"

#include <QCheckBox>
#include <QDebug>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>

#define SETTING_SCROBBLE_POINT "scrobblePoint"
#define SETTING_ALLOW_FINGERPRINTING "allowFingerprint"
#define SETTING_AUDIOBOOKS "audiobooks"
#define SETTING_PODCASTS "podcasts"


ScrobbleSettingsWidget::ScrobbleSettingsWidget( QWidget* parent )
    : SettingsWidget( parent ),
      ui( new Ui::ScrobbleSettingsWidget )
{
    ui->setupUi( this );

    connect( ui->scrobblePoint, SIGNAL(sliderMoved(int)), SLOT(onSliderMoved(int)) );
    connect( ui->scrobblePoint, SIGNAL(sliderMoved(int)), SLOT(onSettingsChanged()) );
    connect( ui->allowFingerprint, SIGNAL(stateChanged(int)), SLOT(onSettingsChanged()) );

    connect( ui->audiobooks, SIGNAL(stateChanged(int)), SLOT(onSettingsChanged()) );
    connect( ui->podcasts, SIGNAL(stateChanged(int)), SLOT(onSettingsChanged()) );


    int scrobblePointValue = unicorn::UserSettings().value( SETTING_SCROBBLE_POINT, ui->scrobblePoint->value() ).toInt();
    ui->scrobblePoint->setValue( scrobblePointValue );
    onSliderMoved( scrobblePointValue );
    ui->percentText->setFixedWidth( ui->percentText->fontMetrics().width( "100" ) );

    ui->allowFingerprint->setChecked( unicorn::UserSettings().value( SETTING_ALLOW_FINGERPRINTING, ui->allowFingerprint->isChecked() ).toBool() );

    ui->audiobooks->setChecked( unicorn::UserSettings().value( SETTING_AUDIOBOOKS, ui->audiobooks->isChecked() ).toBool() );
    ui->podcasts->setChecked( unicorn::UserSettings().value( SETTING_PODCASTS, ui->podcasts->isChecked() ).toBool() );

    ui->line->setFrameShape( QFrame::HLine );
}

void
ScrobbleSettingsWidget::onSliderMoved( int value )
{
    ui->percentText->setText( QString::number( value ) );
}


void
ScrobbleSettingsWidget::saveSettings()
{
    if ( hasUnsavedChanges() )
    {
        qDebug() << "Saving settings...";

        unicorn::UserSettings().setValue( SETTING_SCROBBLE_POINT, ui->scrobblePoint->value() );
        unicorn::UserSettings().setValue( SETTING_ALLOW_FINGERPRINTING, ui->allowFingerprint->isChecked() );
        unicorn::UserSettings().setValue( SETTING_AUDIOBOOKS, ui->audiobooks->isChecked() );
        unicorn::UserSettings().setValue( SETTING_PODCASTS, ui->podcasts->isChecked() );

        onSettingsSaved();
    }
}
