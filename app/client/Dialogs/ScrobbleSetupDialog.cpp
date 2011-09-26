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

#include <QPushButton>
#include <QFile>

#include "ScrobbleSetupDialog.h"
#include "ui_ScrobbleSetupDialog.h"

ScrobbleSetupDialog::ScrobbleSetupDialog( QString deviceId, QString deviceName, QStringList iPodFiles, QWidget* parent )
    :unicorn::Dialog( parent ),
    ui( new Ui::ScrobbleSetupDialog ),
    m_deviceId( deviceId ),
    m_deviceName( deviceName ),
    m_iPodFiles( iPodFiles ),
    m_more( false )
{
    ui->setupUi( this );

    setWindowTitle( "" );

    ui->description->setText( tr("<p>Do you want to start scrobbling the iPod \"%1\"?<p>"
                                     "<p>This will automatically scrobble tracks you've played on your iPod to the user account <strong>\"%2\"</strong> everytime it's synced with iTunes.</p>" ).arg( deviceName, User().name() ) );

    ui->multiUser->setText( tr( "<p>How would you like to manage scrobbling from iPod \"%1\"?</p>" ).arg( deviceName ) );
    ui->always->setChecked( true );

    ui->moreFrame->hide();

    QList<lastfm::User> roster = unicorn::Settings().userRoster();

    if ( roster.count() > 1 )
    {
        ui->multiUser->setText( tr( "<p>Not %1? Scrobble this iPod to a different user:</p>" ).arg( User().name() ) );


        for ( int i = 0 ; i < roster.count() ; i++ )
        {
            ui->users->addItem( roster.at( i ).name() );

            if ( roster.at( i ).name() == User().name() )
                ui->users->setCurrentIndex( i );
        }
    }
    else
    {
        ui->users->hide();
        ui->multiUser->hide();
    }

    ui->buttonBox->button( QDialogButtonBox::Apply )->setText( tr( "More Options" ) );
    ui->buttonBox->button( QDialogButtonBox::Yes )->setText( tr( "Scrobble" ) );
    ui->buttonBox->button( QDialogButtonBox::No )->setText( tr( "Not Now" ) );

    connect( ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), SLOT(onClicked(QAbstractButton*)));
}

ScrobbleSetupDialog::~ScrobbleSetupDialog()
{
    delete ui;
}

void
ScrobbleSetupDialog::onClicked( QAbstractButton* button )
{
    switch ( ui->buttonBox->buttonRole( button ) )
    {
    case QDialogButtonBox::YesRole:
    {
        QString user = User().name();

        if ( ui->users->isVisible() )
            user = ui->users->currentText();

        if ( m_more )
        {
            if ( ui->always->isChecked() )
                emit clicked( true, false, user, m_deviceId, m_deviceName, m_iPodFiles );
            else if ( ui->manual->isChecked() )
                emit clicked( true, true, user, m_deviceId, m_deviceName, m_iPodFiles );
            else if ( ui->never )
                emit clicked( false, false, user, m_deviceId, m_deviceName, m_iPodFiles );
        }
        else
            emit clicked( true, false, user, m_deviceId, m_deviceName, m_iPodFiles );

        done( 0 );

        break;
    }
    case QDialogButtonBox::ApplyRole:

        // remove all the buttons
        foreach ( QAbstractButton* button, ui->buttonBox->buttons() )
            ui->buttonBox->removeButton( button );

        // add new awesome buttons
        ui->buttonBox->addButton( QDialogButtonBox::Ok );
        ui->buttonBox->addButton( QDialogButtonBox::Cancel );

        // show the correct frame
        ui->basicFrame->hide();
        ui->moreFrame->show();

        ui->moreFrame->setFixedHeight( ui->moreFrame->sizeHint().height() );

        m_more = true;

        break;
    case QDialogButtonBox::NoRole:
    case QDialogButtonBox::RejectRole:
    default:
        // just close the dialog, they will be asked all these thingsthe next time

        foreach ( const QString& iPodFile, m_iPodFiles )
            QFile::remove( iPodFile );

        done( 0 );

        break;
    }
}
