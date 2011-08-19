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

ScrobbleSettingsWidget::ScrobbleSettingsWidget( QWidget* parent )
    : SettingsWidget( parent )
{
    setupUi();
    connect( ui.scrobblePoint, SIGNAL( sliderMoved( int ) ), this, SLOT( onSliderMoved( int ) ) );
    connect( ui.scrobblePoint, SIGNAL( sliderMoved( int ) ), this, SLOT( onSettingsChanged() ) );
    connect( ui.allowFingerprint, SIGNAL( stateChanged( int ) ), this, SLOT( onSettingsChanged() ) );

#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
    connect( ui.launchItunes, SIGNAL( stateChanged( int ) ), this, SLOT( onSettingsChanged() ) );
#endif

}

void
ScrobbleSettingsWidget::setupUi()
{
    QVBoxLayout* v = new QVBoxLayout( this );
    QHBoxLayout* h = new QHBoxLayout;

    h->addWidget( new QLabel( tr( "Scrobble at" ), this ) );

    int scrobblePointValue = unicorn::UserSettings().value( "scrobblePoint", 50 ).toInt();

    h->addWidget( ui.scrobblePoint = new QSlider( Qt::Horizontal, this ) );
    ui.scrobblePoint->setValue( scrobblePointValue );
    ui.scrobblePoint->setTickInterval( 10 );
    ui.scrobblePoint->setRange( 50, 100 );
    ui.scrobblePoint->setTickPosition( QSlider::TicksBelow );

    h->addWidget( ui.percentText = new QLabel( QString::number( scrobblePointValue ), this ) );
    int maxWidth = ui.percentText->fontMetrics().width( "100" );
    ui.percentText->setFixedWidth( maxWidth );

    h->addWidget( new QLabel( tr( "percent of track length" ), this ) );

    QGroupBox* groupBox = new QGroupBox( tr( "Preferences" ), this );
    QVBoxLayout* vg = new QVBoxLayout( groupBox );
    vg->addLayout( h );
    vg->addWidget( ui.allowFingerprint = new QCheckBox( tr( "Allow Last.fm to fingerprint your tracks" ), this ) );
    ui.allowFingerprint->setChecked( unicorn::UserSettings().value( "allowFingerprint", false ).toBool() );

#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
    vg->addWidget( ui.launchItunes = new QCheckBox( tr( "Launch iTunes" ), this ) );
    ui.launchItunes->setChecked( unicorn::AppSettings().value( "launchItunes", true ).toBool() );
#endif

    v->addWidget( new QLabel( tr( "Configure Scrobbler Settings" ), this ) );
    v->addWidget( ui.line = new QFrame( this ) );
    ui.line->setFrameShape( QFrame::HLine );
    v->addWidget( groupBox );
    v->addStretch( 1 );
}

void
ScrobbleSettingsWidget::onSliderMoved( int value )
{
    ui.percentText->setText( QString::number( value ) );
}


void
ScrobbleSettingsWidget::saveSettings()
{
    if ( hasUnsavedChanges() )
    {
        qDebug() << "Saving settings...";
        unicorn::UserSettings().setValue( "scrobblePoint", ui.scrobblePoint->value() );
        unicorn::UserSettings().setValue( "allowFingerprint", ui.allowFingerprint->isChecked() );
#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
        unicorn::AppSettings().setValue( "launchItunes", ui.launchItunes->isChecked() );
#endif
        onSettingsSaved();
    }
}
