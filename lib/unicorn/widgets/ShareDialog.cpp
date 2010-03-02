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
#include "ShareDialog.h"
#include "RecipientsWidget.h"
#include "lib/unicorn/widgets/TrackWidget.h"
#include <lastfm/User>
#include <QLineEdit>
#include <QPainter>
#include <QTimer>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QEvent>
#include <QRadioButton>
#include <QComboBox>
#include <QListWidget>


ShareDialog::ShareDialog( const Track& t, QWidget* parent )
        : QDialog( parent )
{
    m_track = t;

    setupUi();
    setWindowTitle( tr("Share") );    
    enableDisableOk();
    
    connect( ui.buttons, SIGNAL(accepted()), SLOT(accept()) );
    connect( ui.buttons, SIGNAL(rejected()), SLOT(reject()) );
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

    QVBoxLayout* v1 = new QVBoxLayout;
    
    v1->addWidget( new QLabel( tr("To:") ) );
    v1->addWidget( ui.recipients = new RecipientsWidget(this) );
    v1->setSpacing( 0 );

    connect( ui.recipients, SIGNAL(changed()), SLOT(enableDisableOk()));

    QVBoxLayout* v2 = new QVBoxLayout;
    v2->addWidget( new QLabel( tr("Message (optional)") ) );
    v2->addWidget( ui.message = new QTextEdit );
    v2->setSpacing( 4 );
    
    QVBoxLayout* v = new QVBoxLayout( this );
    v->addLayout( h1 );
    v->addLayout( v1 );
    v->addLayout( v2 );
    v->addWidget( ui.buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel ) );
    
    ui.message->setAttribute( Qt::WA_MacShowFocusRect, true );
    
    ui.buttons->button( QDialogButtonBox::Ok )->setText( tr("Share") );
    
#ifdef Q_WS_MAC
    foreach (QLabel* l, findChildren<QLabel*>())
        l->setAttribute( Qt::WA_MacSmallSize );
#endif
}

void
ShareDialog::onRadioButtonsClicked( bool )
{
    // which radio button is checked

    if ( ui.artistShare->isChecked() ) ui.track->setType( TrackWidget::Artist );
    else if ( ui.albumShare->isChecked() ) ui.track->setType( TrackWidget::Album );
    else if ( ui.trackShare->isChecked() ) ui.track->setType( TrackWidget::Track );
}

void
ShareDialog::enableDisableOk()
{
    ok()->setEnabled( ui.recipients->recipients().count() > 0 );
}


void
ShareDialog::accept()
{
    QStringList recipients( ui.recipients->recipients() );
    QString const message = ui.message->toPlainText();

    if ( ui.artistShare->isChecked() )
    {
        m_track.artist().share( recipients, message );
    }
    else if ( ui.albumShare->isChecked() )
    {
        m_track.album().share( recipients, message );
    }
    else
    {
        m_track.share( recipients, message );
    }



    //TODO feedback on success etc, do via that bar thing you planned

    QDialog::accept();
}

