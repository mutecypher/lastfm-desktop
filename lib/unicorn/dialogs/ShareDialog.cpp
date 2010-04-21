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

#include "lib/unicorn/widgets/TrackWidget.h"

#include "ShareDialog.h"
#include "../widgets/ItemSelectorWidget.h"
#include "../widgets/SelectedItemWidget.h"

const int kMaxMessage(1000);

ShareDialog::ShareDialog( const Track& t, QWidget* parent )
    : unicorn::Dialog( parent, Qt::Window )
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
    QHBoxLayout* h1 = new QHBoxLayout;
    h1->addWidget( ui.track = new TrackWidget( m_track ), Qt::AlignLeft );

    QVBoxLayout* v1 = new QVBoxLayout;
    
    v1->addWidget( new QLabel( tr("To") ) );
    v1->addWidget( ui.recipients = new ItemSelectorWidget(ItemSelectorWidget::User, this) );
    v1->addWidget( new QLabel( tr("Type friends or emails (up to 10), separated by commas.") ) );
    v1->setSpacing( 0 );

    connect( ui.recipients, SIGNAL(changed()), SLOT(enableDisableOk()));

    QVBoxLayout* v2 = new QVBoxLayout;
    v2->addWidget( new QLabel( tr("Message (optional)") ) );
    v2->addWidget( ui.message = new QPlainTextEdit );
    ui.message->setAttribute( Qt::WA_MacShowFocusRect, false );
    connect( ui.message , SIGNAL(textChanged()), SLOT(onMessageChanged()));
    connect( ui.message , SIGNAL(textChanged()), SLOT(enableDisableOk()));
    v2->setSpacing( 4 );

    QHBoxLayout* h2 = new QHBoxLayout;
    h2->addWidget( ui.isPublic = new QCheckBox( tr("Include in my recent activity"), this ) );
    ui.isPublic->setChecked( true );
    h2->addWidget( ui.characterLimit = new QLabel( this ), 0, Qt::AlignRight );
    updateCharacterLimit();
    
    QVBoxLayout* v = new QVBoxLayout( this );
    v->addWidget( new QLabel( tr("Choose something to share:") ) );
    v->addLayout( h1 );
    v->addLayout( v1 );
    v->addLayout( v2 );
    v->addLayout( h2 );
    v->addWidget( ui.buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel ) );
    
    // make sure that pressing enter doesn't complete the dialog
    ui.buttons->button( QDialogButtonBox::Ok )->setText( tr("Share") );
    
#ifdef Q_WS_MAC
    foreach (QLabel* l, findChildren<QLabel*>())
        l->setAttribute( Qt::WA_MacSmallSize );
#endif

    //tab order
    setTabOrder( ui.recipients, ui.message );
    setTabOrder( ui.message, ui.buttons );
}

void
ShareDialog::enableDisableOk()
{
    ok()->setEnabled( ui.recipients->items().count() > 0
                      && ui.message->toPlainText().length() <= kMaxMessage );
}

void
ShareDialog::updateCharacterLimit()
{
    ui.characterLimit->setText( QString::number( ui.message->toPlainText().length() ) + "/" + QString::number(kMaxMessage) );

    if ( ui.message->toPlainText().length() > kMaxMessage ) {
        ui.characterLimit->setProperty( "xerror", QVariant( true ) );
    }
    else
        ui.characterLimit->setProperty( "xerror", QVariant( false ) );

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
    QStringList recipients( ui.recipients->items() );
    QString const message = ui.message->toPlainText();
    bool isPublic = ui.isPublic->isChecked();

    if ( ui.track->type() == TrackWidget::Artist )
        m_track.artist().share( recipients, message, isPublic );
    else if ( ui.track->type() == TrackWidget::Album )
        m_track.album().share( recipients, message, isPublic );
    else
        m_track.share( recipients, message, isPublic );

    //TODO feedback on success etc, do via that bar thing you planned
    //QDialog::accept();
    close();
}

