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

#include "AccountSettingsWidget.h"

#include "lib/unicorn/QMessageBoxBuilder.h"
#include "lib/unicorn/UnicornApplication.h"
#include "lib/unicorn/UnicornSession.h"
#include "lib/unicorn/UnicornSettings.h"
#include "lib/unicorn/widgets/UserManagerWidget.h"

#include <lastfm/User.h>

#include <QApplication>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

using lastfm::User;


AccountSettingsWidget::AccountSettingsWidget( QWidget* parent )
    : SettingsWidget( parent )
{
    setupUi();
    
    connect( ui.users, SIGNAL( userChanged() ), this, SLOT( onSettingsChanged() ) );
    connect( ui.users, SIGNAL( rosterUpdated() ), qApp, SIGNAL( rosterUpdated() ) );
}

void
AccountSettingsWidget::setupUi()
{
    QLabel* title = new QLabel( tr( "Configure Account Settings" ), this );
    QFrame* line = new QFrame( this );
    QGroupBox* groupBox = new QGroupBox( this );

    line->setFrameShape( QFrame::HLine );

    ui.users = new UserManagerWidget( this );
    QVBoxLayout* v = new QVBoxLayout();
    QVBoxLayout* vb = new QVBoxLayout();
    QHBoxLayout* h = new QHBoxLayout();

    groupBox->setTitle( tr( "Preferences" ) );

    h->addStretch( 1 );

    vb->addLayout( h );

    groupBox->setLayout( vb );

    v->addWidget( title );
    v->addWidget( line );
    v->addWidget( qobject_cast<QWidget* >( ui.users ) );
    v->addWidget( groupBox );

    setLayout( v );
}

void
AccountSettingsWidget::saveSettings()
{
    qDebug() << "has unsaved changes?" << hasUnsavedChanges();
    if ( hasUnsavedChanges() )
    {
        unicorn::Settings s;
        UserRadioButton* urb = qobject_cast<UserRadioButton*>( ui.users->checkedButton() );
        if ( urb && urb->user() != User().name() )
        {
            s.setValue( "Username", urb->user() );
            s.beginGroup( urb->user() );
            QString sessionKey = s.value( "SessionKey", "" ).toString();
            s.endGroup();
            qobject_cast<unicorn::Application *>( qApp )->changeSession( urb->user(), sessionKey, true );
        }

        onSettingsSaved();
    }
}
