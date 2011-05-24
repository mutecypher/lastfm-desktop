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

#include "ScrobbleSetupDialog.h"
#include "ui_ScrobbleSetupDialog.h"

ScrobbleSetupDialog::ScrobbleSetupDialog( QString deviceId, QString deviceName, QStringList iPodFiles, QWidget* parent )
    :unicorn::Dialog( parent ),
    ui( new Ui::ScrobbleSetupDialog ),
    m_deviceId( deviceId ),
    m_deviceName( deviceName ),
    m_iPodFiles( iPodFiles )
{
    ui->setupUi(this);

//    vl->addWidget( ui.title = new QLabel( tr("Do you want to scrobble the iPod '%1'?").arg( "Coffey's iPod" )  ) );
//    ui.title->setObjectName("title");

//    vl->addWidget( ui.description = new QLabel( tr("This will automatically scrobble tracks you've played on your iPod everytime it's connected.") ) );
//    ui.description->setObjectName("description");

    connect( ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), SLOT(onClicked(QAbstractButton*)));
}

ScrobbleSetupDialog::~ScrobbleSetupDialog()
{
    delete ui;
}

void
ScrobbleSetupDialog::onClicked( QAbstractButton* button )
{
    IpodDevice::Scrobble buttonPressed = IpodDevice::Yes;

    QDialogButtonBox::ButtonRole buttonRole = ui->buttonBox->buttonRole( button );

    if ( buttonRole == QDialogButtonBox::RejectRole )
        buttonPressed = IpodDevice::NotNow;
    else if ( buttonRole == QDialogButtonBox::NoRole )
        buttonPressed = IpodDevice::Never;

    emit clicked( buttonPressed, m_deviceId, m_deviceName, m_iPodFiles );

    done( 0 );
}
