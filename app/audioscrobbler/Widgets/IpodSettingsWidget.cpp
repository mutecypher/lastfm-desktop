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

#include "IpodSettingsWidget.h"

#include "lib/unicorn/UnicornSettings.h"

#include <QCheckBox>
#include <QDebug>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QTreeWidget>
#include <QVBoxLayout>

IpodSettingsWidget::IpodSettingsWidget( QWidget* parent )
    : SettingsWidget( parent )
{
    setupUi();

#ifdef Q_OS_MAC || Q_OS_WIN
    connect( ui.enableScrobbling, SIGNAL( stateChanged( int ) ), this, SLOT( onSettingsChanged() ) );
#endif

    connect( ui.confirmScrobbles, SIGNAL( stateChanged( int ) ), this, SLOT( onSettingsChanged() ) );
    connect( ui.clearAssociations, SIGNAL( clicked() ), this, SLOT( clearIpodAssociations() ) );
}


void
IpodSettingsWidget::setupUi()
{
    QLabel* title = new QLabel( tr( "Configure Media Devices" ), this );
    QFrame* line = new QFrame( this );
    line->setFrameShape( QFrame::HLine );

    QVBoxLayout* v = new QVBoxLayout;
    QVBoxLayout* vg = new QVBoxLayout;
    QVBoxLayout* vg2 = new QVBoxLayout;
    QHBoxLayout* h1 = new QHBoxLayout;

    ui.confirmScrobbles = new QCheckBox( this );
    ui.iPodAssociations = new QTreeWidget( this );
    ui.clearAssociations = new QPushButton( this );
    QGroupBox* groupBox1 = new QGroupBox( this );
    QGroupBox* groupBox2 = new QGroupBox( this );

   // groupBox1->setTitle( tr( "Configure " ) );
    groupBox2->setTitle( tr( "iPod Associations" ) );

#ifdef Q_OS_MAC || Q_OS_WIN
    ui.enableScrobbling = new QCheckBox( this );
    ui.enableScrobbling->setText( tr( "Enable iPod scrobbling" ) );
    ui.enableScrobbling->setChecked( unicorn::UserSettings().value( "enableIpodScrobbling", true ).toBool() );
    vg->addWidget( ui.enableScrobbling );
#endif

    ui.confirmScrobbles->setText( tr( "Always confirm iPod scrobbles" ) );

    ui.confirmScrobbles->setChecked( unicorn::UserSettings().value( "confirmIpodScrobbles", false ).toBool() );

    vg->addWidget( ui.confirmScrobbles );

    groupBox1->setLayout( vg );


    ui.iPodAssociations->setColumnCount( 2 );
    ui.iPodAssociations->setSortingEnabled( false );
    ui.iPodAssociations->header()->setResizeMode( QHeaderView::ResizeToContents );
    ui.iPodAssociations->setRootIsDecorated( false );
    ui.iPodAssociations->setUniformRowHeights( true );
    ui.iPodAssociations->setItemsExpandable( false );
    ui.iPodAssociations->setAllColumnsShowFocus( true );
    ui.iPodAssociations->setStyleSheet( "color: black; " );

    populateIpodAssociations();

    QStringList headerLabels;
    headerLabels.append( tr( "Device" ) );
    headerLabels.append( tr( "User" ) );
    ui.iPodAssociations->setHeaderLabels( headerLabels );

    ui.clearAssociations->setText( tr( "Clear user associations" ) );

    h1->addStretch( 1 );
    h1->addWidget( ui.clearAssociations );
    vg2->addWidget( ui.iPodAssociations );
    vg2->addLayout( h1 );

    groupBox2->setLayout( vg2 );

    v->addWidget( title );
    v->addWidget( line );
    v->addWidget( groupBox1 );
    v->addWidget( groupBox2 );
    v->addStretch( 1 );
    setLayout( v );

}

void
IpodSettingsWidget::saveSettings()
{
    if ( hasUnsavedChanges() )
    {
        //save settings
        qDebug() << "Saving settings...";
#ifdef Q_OS_MAC || Q_OS_WIN
        unicorn::UserSettings().setValue( "enableIpodScrobbling", ui.enableScrobbling->isChecked() );
#endif
        unicorn::UserSettings().setValue( "confirmIpodScrobbles", ui.confirmScrobbles->isChecked() );
        onSettingsSaved();
    }
}

void
IpodSettingsWidget::populateIpodAssociations()
{
    //TODO: read from settings

    ui.clearAssociations->setEnabled( ui.iPodAssociations->topLevelItemCount() > 0 );
}

void
IpodSettingsWidget::clearIpodAssociations()
{
   ui.iPodAssociations->clear();
   ui.clearAssociations->setEnabled( false );
}
