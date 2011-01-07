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

#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>

#include "ScrobbleSetupDialog.h"

ScrobbleSetupDialog::ScrobbleSetupDialog( QString iPodFile, QWidget *parent )
    :Dialog( parent ), m_iPodFile( iPodFile )
{
    QVBoxLayout* layout = new QVBoxLayout( this );

    QHBoxLayout* hl = new QHBoxLayout();
    hl->addWidget( ui.iPod = new QLabel("") );
    ui.iPod->setObjectName("iPod");

    QVBoxLayout* vl = new QVBoxLayout();
    vl->addWidget( ui.title = new QLabel( tr("Do you want to scrobble the iPod '%1'?").arg( "Coffey's iPod" )  ) );
    ui.title->setObjectName("title");

    vl->addWidget( ui.description = new QLabel( tr("This will automatically scrobble tracks you've played on your iPod everytime it's connected.") ) );
    ui.description->setObjectName("description");

    vl->addWidget( ui.dontRemind = new QCheckBox( tr("Don't remind me again") ) );
    ui.dontRemind->setObjectName("dontRemind");

    hl->addLayout( vl );

    layout->addLayout( hl );

    layout->addWidget( ui.buttons = new QDialogButtonBox() );
    ui.buttons->setObjectName("buttons");

    ui.buttons->addButton( tr("Yes"), QDialogButtonBox::YesRole );
    ui.buttons->addButton( tr("Not now"), QDialogButtonBox::RejectRole );
    ui.buttons->addButton( tr("Never"), QDialogButtonBox::NoRole );
}

void
ScrobbleSetupDialog::onClicked( QAbstractButton* button )
{
    Button buttonPressed = Yes;

    QDialogButtonBox::ButtonRole buttonRole = ui.buttons->buttonRole( button );

    if ( buttonRole == QDialogButtonBox::RejectRole )
        buttonPressed = NotNow;
    else if ( QDialogButtonBox::NoRole )
        buttonPressed = Never;

    emit clicked( buttonPressed, m_iPodFile );
}
