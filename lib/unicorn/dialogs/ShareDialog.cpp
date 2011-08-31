/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

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

#include <QApplication>
#include <QPainter>
#include <QTimer>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QEvent>
#include <QRadioButton>
#include <QComboBox>
#include <QListWidget>
#include <QDebug>
#include <QCheckBox>
#include <QDesktopWidget>

#include <lastfm/User>
#include <lastfm/XmlQuery>

#include "lib/unicorn/widgets/TrackWidget.h"

#include "ShareDialog.h"

#include "ui_ShareDialog.h"

#include "../widgets/ItemSelectorWidget.h"

const int kMaxMessage(1000);

ShareDialog::ShareDialog( const Track& t, QWidget* parent )
    : unicorn::Dialog( parent, Qt::Tool ),
      ui( new Ui::ShareDialog ),
      m_track( t )
{
    ui->setupUi( this );

    ui->recipients->setType( ItemSelectorWidget::User );
    ui->icon->loadUrl( m_track.imageUrl( lastfm::Small, true ), false );

    ui->album->setEnabled( !t.album().isNull() );

    onRadioButtonClicked();
    connect( ui->track, SIGNAL(clicked()), SLOT(onRadioButtonClicked()) );
    connect( ui->artist, SIGNAL(clicked()), SLOT(onRadioButtonClicked()) );
    connect( ui->album, SIGNAL(clicked()), SLOT(onRadioButtonClicked()) );

    setWindowTitle( tr("Share") );    
    enableDisableOk();

    connect( ui->buttons, SIGNAL(accepted()), SLOT(accept()) );
    connect( ui->buttons, SIGNAL(rejected()), SLOT(close()) );

    connect( ui->message, SIGNAL(textChanged()), SLOT(updateCharacterLimit()));
    connect( ui->message, SIGNAL(textChanged()), SLOT(enableDisableOk()));
    connect( ui->recipients, SIGNAL(changed()), SLOT(enableDisableOk()));


}

void
ShareDialog::onRadioButtonClicked()
{
    if ( ui->track->isChecked() )
    {
        ui->shareSubject->setText( m_track.title() );
        ui->shareSubject2->setText( tr( "by %1" ).arg( m_track.artist() ) );
    }
    else if ( ui->artist->isChecked() )
    {
        ui->shareSubject->setText( m_track.artist() );
        ui->shareSubject2->setText( "" );
    }
    else if ( ui->album->isChecked() )
    {
        ui->shareSubject->setText( m_track.album() );
        ui->shareSubject2->setText( tr( "by %1" ).arg( m_track.artist() ) );
    }
}

void
ShareDialog::enableDisableOk()
{
    ui->buttons->button( QDialogButtonBox::Ok )->setEnabled( ui->recipients->items().count() > 0
                      && ui->message->toPlainText().length() <= kMaxMessage );
}

void
ShareDialog::updateCharacterLimit()
{
    ui->characterLimit->setText( QString::number( ui->message->toPlainText().length() ) + "/" + QString::number(kMaxMessage) );

    if ( ui->message->toPlainText().length() > kMaxMessage ) {
        ui->characterLimit->setProperty( "xerror", true );
    }
    else
        ui->characterLimit->setProperty( "xerror", false );

    style()->polish( ui->characterLimit );
}

void
ShareDialog::setTabOrders()
{
//    setTabOrder( ui->track, ui->recipients );
//    setTabOrder( ui->recipients, ui->message );
//    setTabOrder( ui->message, ui->isPublic );
//    setTabOrder( ui->isPublic, ui->buttons->button( QDialogButtonBox::Ok ) );
//    setTabOrder( ui->buttons->button( QDialogButtonBox::Ok ), ui->buttons->button( QDialogButtonBox::Cancel ) );
}

void
ShareDialog::onMessageChanged()
{
    // update the character message
    updateCharacterLimit();
}

void
ShareDialog::accept()
{
    QStringList recipients( ui->recipients->items() );
    QString const message = ui->message->toPlainText();
    bool isPublic = ui->isPublic->isChecked();

    QNetworkReply* reply = 0;

    if ( ui->track->isChecked() )
        reply = m_track.share( recipients, message, isPublic ) ;
    else if ( ui->album->isChecked() )
        reply = m_track.album().share( recipients, message, isPublic );
    else
        reply = m_track.artist().share( recipients, message, isPublic );

    // disable the dialog until we get a response from Last.fm
    setEnabled( false );
    connect( reply, SIGNAL(finished()), SLOT(onShared()) );
}

void
ShareDialog::onShared()
{
    try
    {
        XmlQuery lfm = qobject_cast<QNetworkReply*>(sender())->readAll();

        if ( lfm.attribute( "status" ) == "ok" )
            close();
        else
        {
            // TODO: display some kind of error message
            setEnabled( true );
        }
    }
    catch ( ... )
    {
        // TODO: display some kind of error message
        setEnabled( true );
    }
}

