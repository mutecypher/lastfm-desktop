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

#ifdef Q_WS_X11
#include "../MediaDevices/IpodDevice_linux.h"
#endif

#include "lib/unicorn/UnicornSettings.h"

#include <lastfm/User>

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

enum
{
    IpodColumnUser,
    IpodColumnDeviceName,
    IpodColumnCount
};

IpodSettingsWidget::IpodSettingsWidget( QWidget* parent )
    : SettingsWidget( parent )
{
    setupUi();

#ifdef Q_OS_MAC || Q_OS_WIN
    connect( ui.enableScrobbling, SIGNAL( stateChanged( int ) ), this, SLOT( onSettingsChanged() ) );
#endif

    connect( ui.confirmScrobbles, SIGNAL( stateChanged( int ) ), this, SLOT( onSettingsChanged() ) );
    connect( ui.clearAssociations, SIGNAL( clicked() ), this, SLOT( clearIpodAssociations() ) );
    connect( ui.removeAssociation, SIGNAL( clicked() ), this, SLOT( removeIpodAssociation() ) );
    connect( ui.iPodAssociations, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( onItemActivated() ) );
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
    ui.removeAssociation = new QPushButton( this );

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


    ui.iPodAssociations->setColumnCount( IpodColumnCount );
    ui.iPodAssociations->setSortingEnabled( false );
    ui.iPodAssociations->header()->setResizeMode( QHeaderView::ResizeToContents );
    ui.iPodAssociations->setRootIsDecorated( false );
    ui.iPodAssociations->setUniformRowHeights( true );
    ui.iPodAssociations->setItemsExpandable( false );
    ui.iPodAssociations->setAllColumnsShowFocus( true );
    ui.iPodAssociations->setStyleSheet( "color: black; " );

    populateIpodAssociations();

    QStringList headerLabels;
    headerLabels.append( tr( "User" ) );
    headerLabels.append( tr( "Device Name" ) );

    ui.iPodAssociations->setHeaderLabels( headerLabels );

    ui.clearAssociations->setText( tr( "Clear user associations" ) );
    ui.removeAssociation->setText( tr( "Remove association" ) );

    h1->addStretch( 1 );
    h1->addWidget( ui.removeAssociation );
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
    QList<lastfm::User> roster = unicorn::Settings().userRoster();
    foreach( lastfm::User user, roster )
    {
        unicorn::UserSettings us( user.name() );
        int count = us.beginReadArray( "associatedDevices" );

        for ( int i = 0; i < count; i++ )
        {
            us.setArrayIndex( i );
            QTreeWidgetItem* item = new QTreeWidgetItem( ui.iPodAssociations );
            item->setText( IpodColumnUser, user.name() );
            item->setText( IpodColumnDeviceName, us.value( "deviceName" ).toString() );
            item->setData( IpodColumnDeviceName, Qt::UserRole, us.value( "deviceId" ).toString() );
        }
        us.endArray();
    }

    ui.clearAssociations->setEnabled( ui.iPodAssociations->topLevelItemCount() > 0 );
    ui.removeAssociation->setEnabled( false );
}

void
IpodSettingsWidget::clearIpodAssociations()
{
    QList<lastfm::User> roster = unicorn::Settings().userRoster();
    foreach( lastfm::User user, roster )
    {
        unicorn::UserSettings us( user.name() );
        us.remove( "associatedDevices" );
    }

#ifdef Q_WS_X11
    IpodDevice::deleteDevicesHistory();
#endif

    ui.iPodAssociations->clear();
    ui.clearAssociations->setEnabled( false );
    ui.removeAssociation->setEnabled( false );
}

void
IpodSettingsWidget::onItemActivated()
{

    ui.removeAssociation->setEnabled( true );
}

void
IpodSettingsWidget::removeIpodAssociation()
{
    QTreeWidgetItem* association = ui.iPodAssociations->currentItem();
    QString deviceId = association->data( IpodColumnDeviceName, Qt::UserRole ).toString();
    QString userName = association->text( IpodColumnUser );

    unicorn::UserSettings us( userName );
    int count = us.beginReadArray( "associatedDevices" );
    for ( int i = 0; i < count; i++ )
    {
        us.setArrayIndex( i );
        if ( deviceId == us.value( "deviceId" ).toString() )
        {
            us.remove( "deviceId" );
            us.remove( "mountPath" );
            us.remove( "deviceName" );
            #ifdef Q_WS_X11
            IpodDevice::deleteDeviceHistory( userName, deviceId );
            #endif
            break;
        }
    }
    us.endArray();
    us.setValue( "associatedDevices/size", count - 1 );
    ui.iPodAssociations->takeTopLevelItem( ui.iPodAssociations->indexOfTopLevelItem( association ) );
    ui.removeAssociation->setEnabled( false );
}
