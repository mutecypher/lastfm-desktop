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

#include "SettingsDialog.h"
#include "../Widgets/AccountSettingsWidget.h"
#include "../Widgets/IpodSettingsWidget.h"
#include "../Widgets/ScrobbleSettingsWidget.h"

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QStackedWidget>

SettingsDialog::SettingsDialog( QWidget *parent ) :
    Dialog(parent)
{
    setupUi();
}


void
SettingsDialog::setupUi()
{
    QHBoxLayout* h = new QHBoxLayout;
    ui.pageList = new QListWidget( this );
    ui.pageStack = new QStackedWidget( this );

    QVBoxLayout* containerLayout = new QVBoxLayout;
    containerLayout->addWidget( ui.pageStack );


    ui.pageList->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::MinimumExpanding ) );
    ui.pageList->setFixedWidth( PAGE_LIST_WIDTH );

    ui.pageList->addItem( tr( "Account" ) );
    ui.pageList->setStyleSheet( "color: black;" );
    ui.pageList->addItem( tr( "Scrobbling" ) );
    ui.pageList->addItem( tr( "iPod" ) );
    ui.pageList->setCurrentRow( 0 );

    ui.pageStack->addWidget( ui.accountSettings = new AccountSettingsWidget( this ) );
    ui.pageStack->addWidget( ui.scrobbleSettings = new ScrobbleSettingsWidget( this ) );
    ui.pageStack->addWidget( ui.ipodSettings = new IpodSettingsWidget( this ) );
    ui.pageStack->setCurrentIndex( 0 );

    connect( ui.pageList, SIGNAL( currentRowChanged( int ) ), ui.pageStack, SLOT( setCurrentIndex( int ) ) );

    h->addWidget( ui.pageList );
    h->addLayout( containerLayout );

    QVBoxLayout* v = new QVBoxLayout( this );

    ui.buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply );
    v->addLayout( h );
    v->addWidget( ui.buttons );

    setLayout( v );

    ui.buttons->button( QDialogButtonBox::Apply )->setEnabled( false );

    connect( this, SIGNAL( saveNeeded() ), ui.scrobbleSettings, SLOT( saveSettings() ) );
    connect( this, SIGNAL( saveNeeded() ), ui.ipodSettings, SLOT( saveSettings() ) );
    connect( this, SIGNAL( saveNeeded() ), ui.accountSettings, SLOT( saveSettings() ) );

    connect( ui.scrobbleSettings, SIGNAL( settingsChanged() ), this, SLOT( onSettingsChanged() ) );
    connect( ui.ipodSettings, SIGNAL( settingsChanged() ), this, SLOT( onSettingsChanged() ) );
    connect( ui.accountSettings, SIGNAL( settingsChanged() ), this, SLOT( onSettingsChanged() ) );

    connect( ui.buttons, SIGNAL( accepted() ), this, SLOT( onAccepted() ) );
    connect( ui.buttons, SIGNAL( rejected() ), this, SLOT( reject() ) );
    connect( ui.buttons->button( QDialogButtonBox::Apply ), SIGNAL( clicked() ), this, SLOT( onApplyButtonClicked() ) );
}

void
SettingsDialog::onAccepted()
{
    emit saveNeeded();
    accept();
}

void
SettingsDialog::onSettingsChanged()
{
    ui.buttons->button( QDialogButtonBox::Apply )->setEnabled( true );
}

void
SettingsDialog::onApplyButtonClicked()
{
    emit saveNeeded();
    ui.buttons->button( QDialogButtonBox::Apply )->setEnabled( false );
}
