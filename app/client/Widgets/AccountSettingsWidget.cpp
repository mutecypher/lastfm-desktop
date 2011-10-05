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
    populateLanguages();
    connect( ui.users, SIGNAL( userChanged() ), this, SLOT( onSettingsChanged() ) );
    connect( ui.languages, SIGNAL( currentIndexChanged( int ) ), this, SLOT( onSettingsChanged() ) );
    connect( ui.users, SIGNAL( rosterUpdated() ), qApp, SIGNAL( rosterUpdated() ) );
}

void
AccountSettingsWidget::setupUi()
{
    QLabel* title = new QLabel( tr( "Configure Account Settings" ), this );
    QFrame* line = new QFrame( this );
    QGroupBox* groupBox = new QGroupBox( this );
    QLabel* langLabel = new QLabel( tr( "Language:" ), this );
    ui.languages = new QComboBox( this );
    ui.languages->setStyleSheet( "color: black;" );

    line->setFrameShape( QFrame::HLine );

    ui.users = new UserManagerWidget( this );
    QVBoxLayout* v = new QVBoxLayout();
    QVBoxLayout* vb = new QVBoxLayout();
    QHBoxLayout* h = new QHBoxLayout();

    groupBox->setTitle( tr( "Preferences" ) );

    h->addWidget( langLabel );
    h->addWidget( ui.languages );
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

        int currIndex = ui.languages->currentIndex();
        QString currLanguage = ui.languages->itemData( currIndex ).toString();

        if ( unicorn::AppSettings().value( "language", "" ) != currLanguage )
        {
            unicorn::AppSettings().setValue( "language", currLanguage );
            QMessageBoxBuilder( 0 )
                .setIcon( QMessageBox::Information )
                .setTitle( tr( "Restart needed" ) )
                .setText( tr( "You need to restart the application for the language change to take effect." ) )
                .exec();
        }
        onSettingsSaved();
    }
}

void
AccountSettingsWidget::populateLanguages()
{
    ui.languages->addItem( tr( "System Language" ), "" );
    ui.languages->addItem( "English", QLocale( QLocale::English ).name().left( 2 ) );
    ui.languages->addItem( QString::fromUtf8( "Français" ), QLocale( QLocale::French ).name().left( 2 ) );
    ui.languages->addItem( "Italiano", QLocale( QLocale::Italian ).name().left( 2 )  );
    ui.languages->addItem( "Deutsch", QLocale( QLocale::German ).name().left( 2 ) );
    ui.languages->addItem( QString::fromUtf8( "Español" ), QLocale( QLocale::Spanish ).name().left( 2 ) );
    ui.languages->addItem( QString::fromUtf8( "Portugués" ), QLocale( QLocale::Portuguese ).name().left( 2 ) );
    ui.languages->addItem( "Polski", QLocale( QLocale::Polish ).name().left( 2 ) );
    ui.languages->addItem( "Svenska", QLocale( QLocale::Swedish ).name().left( 2 ) );
    ui.languages->addItem( QString::fromUtf8( "Tükçe" ), QLocale( QLocale::Turkish ).name().left( 2 ) );
    ui.languages->addItem( QString::fromUtf8( "� усский" ), QLocale( QLocale::Russian ).name().left( 2 ) );
    ui.languages->addItem( QString::fromUtf8( "中文" ), QLocale( QLocale::Chinese ).name().left( 2 ) );
    ui.languages->addItem( QString::fromUtf8( "日本語" ), QLocale( QLocale::Japanese ).name().left( 2 ) );

    QString currLanguage = unicorn::AppSettings().value( "language", "" ).toString();
    int index = ui.languages->findData( currLanguage );
    if ( index != -1 )
    {
        ui.languages->setCurrentIndex( index );
    }
}
