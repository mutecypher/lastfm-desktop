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

#include <lastfm/User.h>
#include <lastfm/XmlQuery.h>

#include "lib/unicorn/widgets/TrackWidget.h"
#include "lib/unicorn/DesktopServices.h"

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
    ui->icon->loadUrl( m_track.imageUrl( lastfm::Small, true ), HttpImageWidget::ScaleAuto );

    enableDisableOk();

    connect( ui->message, SIGNAL(textChanged()), SLOT(updateCharacterLimit()));
    connect( ui->message, SIGNAL(textChanged()), SLOT(enableDisableOk()));
    connect( ui->recipients, SIGNAL(changed()), SLOT(enableDisableOk()));

    setWindowTitle( tr("Share") );
}

QString
ShareDialog::shareText( const Track& track )
{
    return tr("%1 by %2").arg( track.title(), track.artist().name() );
}

QUrl
ShareDialog::shareUrl( const Track& track )
{
    return track.www();
}

void
ShareDialog::setShareText()
{
    ui->message->setText( shareText( m_track ) );
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

    // disable the dialog until we get a response from Last.fm
    setEnabled( false );
    connect( m_track.share( recipients, message, isPublic ), SIGNAL(finished()), SLOT(onShared()) );
}

void
ShareDialog::shareTwitter( const Track& track )
{
    QUrl twitterShareIntent( "http://twitter.com/intent/tweet" );
    twitterShareIntent.addEncodedQueryItem( "text", QUrl::toPercentEncoding( shareText( track ) ) );
    twitterShareIntent.addEncodedQueryItem( "url", QUrl::toPercentEncoding( shareUrl( track ).toEncoded() ) );
    twitterShareIntent.addQueryItem( "related", "lastfm" );
    twitterShareIntent.addQueryItem( "hashtags", "lastfm" );
    unicorn::DesktopServices::openUrl( twitterShareIntent );

}

void
ShareDialog::shareFacebook( const Track& track )
{
    QUrl facebookShareIntent( "http://www.facebook.com/sharer.php" );
    facebookShareIntent.addEncodedQueryItem( "t", QUrl::toPercentEncoding( shareText( track ) ) );
    facebookShareIntent.addEncodedQueryItem( "u", QUrl::toPercentEncoding( shareUrl( track ).toEncoded() ) );
    unicorn::DesktopServices::openUrl( facebookShareIntent );
}

void
ShareDialog::onShared()
{
    XmlQuery lfm;

    if ( lfm.parse( qobject_cast<QNetworkReply*>(sender())->readAll() ) )
    {
        if ( lfm.attribute( "status" ) == "ok" )
            close();
        else
        {
            // TODO: display some kind of error message
            setEnabled( true );
        }
    }
    else
    {
        // TODO: display some kind of error message
        setEnabled( true );
    }
}

