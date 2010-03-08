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
#include <QLineEdit>
#include <QPainter>
#include <QTimer>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QEvent>
#include <QRadioButton>
#include <QComboBox>
#include <QListWidget>
#include <QDebug>
#include <QCheckBox>

#include <lastfm/User>

#include "lib/unicorn/widgets/TrackWidget.h"

#include "ShareDialog.h"
#include "RecipientsWidget.h"
#include "RecipientWidget.h"

const int kMaxMessage(1000);

ShareDialog::ShareDialog( const Track& t, QWidget* parent )
    : QWidget( parent, Qt::Window )
{
    m_track = t;

    setupUi();
    setWindowTitle( tr("Share") );    
    enableDisableOk();
    
    connect( ui.buttons, SIGNAL(accepted()), this, SLOT(accept()) );
    connect( ui.buttons, SIGNAL(rejected()), this, SLOT(close()) );
}


void
ShareDialog::setupUi()
{
    // the radio buttons layout
    QVBoxLayout* radioButtons = new QVBoxLayout;

    radioButtons->addWidget( ui.trackShare = new QRadioButton( tr("Track"), this ) );
    radioButtons->addWidget( ui.artistShare = new QRadioButton( tr("Artist"), this ) );
    radioButtons->addWidget( ui.albumShare = new QRadioButton( tr("Album"), this ) );

    connect( ui.artistShare, SIGNAL(clicked(bool)), SLOT(onRadioButtonsClicked(bool)) );
    connect( ui.albumShare, SIGNAL(clicked(bool)), SLOT(onRadioButtonsClicked(bool)) );
    connect( ui.trackShare, SIGNAL(clicked(bool)), SLOT(onRadioButtonsClicked(bool)) );

    // Default to artist sharing
    ui.trackShare->setChecked( true );

    QHBoxLayout* h1 = new QHBoxLayout;
    h1->addLayout( radioButtons );
    h1->addWidget( ui.track = new TrackWidget( m_track ), Qt::AlignLeft );
    ui.track->setCoverHeight( radioButtons->sizeHint().height() );

    QVBoxLayout* v1 = new QVBoxLayout;
    
    v1->addWidget( new QLabel( tr("To:") ) );
    v1->addWidget( ui.recipients = new RecipientsWidget(this) );
    v1->addWidget( new QLabel( tr("Type friends’ names or emails (up to 10) above, separated by commas.") ) );
    v1->setSpacing( 0 );

    connect( ui.recipients, SIGNAL(changed()), SLOT(enableDisableOk()));

    QVBoxLayout* v2 = new QVBoxLayout;
    v2->addWidget( new QLabel( tr("Message (optional)") ) );
    v2->addWidget( ui.message = new QPlainTextEdit );
    connect( ui.message , SIGNAL(textChanged()), SLOT(onMessageChanged()));
    connect( ui.message , SIGNAL(textChanged()), SLOT(enableDisableOk()));
    v2->addWidget( ui.characterLimit = new QLabel( this ) );
    v2->addWidget( ui.isPublic = new QCheckBox( tr("Include in my recent activity"), this ) );
    ui.isPublic->setChecked( true );
    updateCharacterLimit();
    v2->setSpacing( 4 );
    
    QVBoxLayout* v = new QVBoxLayout( this );
    v->addLayout( h1 );
    v->addLayout( v1 );
    v->addLayout( v2 );
    v->addWidget( ui.buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel ) );
    
    ui.message->setAttribute( Qt::WA_MacShowFocusRect, true );
    
    // make sure that pressing enter doesn't complete the dialog
    ui.buttons->button( QDialogButtonBox::Ok )->setText( tr("Share") );
    
#ifdef Q_WS_MAC
    foreach (QLabel* l, findChildren<QLabel*>())
        l->setAttribute( Qt::WA_MacSmallSize );
#endif
}

void
ShareDialog::onRadioButtonsClicked( bool )
{
    // change the share desription to what we are now sharing
    if ( ui.artistShare->isChecked() ) ui.track->setType( TrackWidget::Artist );
    else if ( ui.albumShare->isChecked() ) ui.track->setType( TrackWidget::Album );
    else if ( ui.trackShare->isChecked() ) ui.track->setType( TrackWidget::Track );
}

void
ShareDialog::enableDisableOk()
{
    ok()->setEnabled( ui.recipients->recipients().count() > 0
                      && ui.message->toPlainText().length() <= kMaxMessage );
}

void
ShareDialog::updateCharacterLimit()
{
    ui.characterLimit->setText( QString::number( ui.message->toPlainText().length() ) + "/" + QString::number(kMaxMessage) + " characters used" );

    if ( ui.message->toPlainText().length() > kMaxMessage ) {
        ui.characterLimit->setProperty( "xerror", QVariant( true ) );
    }
    //else if ( ui.message->toPlainText().length() > kMaxMessage - 20 )
    //    ui.characterLimit->setProperty( "status", "warning");
    else
        ui.characterLimit->setProperty( "xerror", QVariant( false ) );

    //ui.characterLimit->style()->unpolish(ui.characterLimit);
    //ui.characterLimit->ensurePolished();

    ui.characterLimit->setStyle(QApplication::style());
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
    QStringList recipients( ui.recipients->recipients() );
    QString const message = ui.message->toPlainText();
    bool isPublic = ui.isPublic->isChecked();

    if ( ui.artistShare->isChecked() ) m_track.artist().share( recipients, message, isPublic );
    else if ( ui.albumShare->isChecked() )  m_track.album().share( recipients, message, isPublic );
    else m_track.share( recipients, message, isPublic );

    //TODO feedback on success etc, do via that bar thing you planned
    //QDialog::accept();
    close();
}

