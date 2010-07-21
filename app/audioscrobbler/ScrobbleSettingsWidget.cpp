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
}

void
ScrobbleSettingsWidget::setupUi()
{
    QLabel* title = new QLabel( tr( "Configure Scrobbler Settings" ), this );
    QFrame* line = new QFrame( this );
    line->setFrameShape( QFrame::HLine );

    QLabel* scrobblePointText = new QLabel( tr( "Scrobble at" ), this );
    QLabel* trackLengthText = new QLabel( tr( "percent of track length" ), this );
    ui.scrobblePoint = new QSlider( Qt::Horizontal, this );
    ui.allowFingerprint = new QCheckBox( this );

    QVBoxLayout* v = new QVBoxLayout;
    QVBoxLayout* vg = new QVBoxLayout;
    QHBoxLayout* h = new QHBoxLayout;

    ui.scrobblePoint->setTickInterval( 10 );
    ui.scrobblePoint->setRange( 50, 100 );
    ui.scrobblePoint->setTickPosition( QSlider::TicksBelow );

    ui.allowFingerprint->setText( tr( "Allow Last.fm to fingerprint your tracks" ) );

    int scrobblePointValue = unicorn::UserSettings().value( "scrobblePoint", 50 ).toInt();

    ui.scrobblePoint->setValue( scrobblePointValue );
    ui.percentText = new QLabel( QString::number( scrobblePointValue ), this );

    int maxWidth = ui.percentText->fontMetrics().width( "100" );
    ui.percentText->setFixedWidth( maxWidth );

    ui.allowFingerprint->setChecked( unicorn::UserSettings().value( "allowFingerprint", false ).toBool() );


    QGroupBox* groupBox = new QGroupBox( this );
    groupBox->setTitle( tr( "Preferences" ) );

    h->addWidget( scrobblePointText );
    h->addSpacing( 5 );
    h->addWidget( ui.scrobblePoint );
    h->addSpacing( 2 );
    h->addWidget( ui.percentText );
    h->addWidget( trackLengthText );

    vg->addLayout( h );
    vg->addWidget( ui.allowFingerprint );

    groupBox->setLayout( vg );

    v->addWidget( title );
    v->addWidget( line );
    v->addWidget( groupBox );
    v->addStretch( 1 );

    setLayout( v );

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
    }
}
