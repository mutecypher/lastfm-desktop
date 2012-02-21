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

#include <QtGui>
#include <QLabel>

#include "lib/unicorn/dialogs/ScrobbleConfirmationDialog.h"

#include "MessageBar.h"

MessageBar::MessageBar( QWidget* parent )
    :StylableWidget( parent )
{
    QHBoxLayout* layout = new QHBoxLayout( this );

    layout->addWidget( ui.icon = new QLabel() );
    ui.icon->setObjectName( "icon" );

    layout->addWidget( ui.message = new QLabel() );
    ui.message->setObjectName( "message" );

    layout->addStretch();

    layout->addWidget( ui.close = new QPushButton() );
    ui.close->setObjectName( "close" );

    connect( ui.message, SIGNAL(linkActivated(QString)), SLOT(onLinkActivated(QString)));
    connect( ui.close, SIGNAL(clicked()), SLOT(hide()));

    connect( qApp, SIGNAL(showMessage(QString,QString)), SLOT(show(QString,QString)));

    hide();
}

void
MessageBar::setTracks( const QList<lastfm::Track>& tracks )
{
    m_tracks = tracks;
}

void
MessageBar::show( const QString& message, const QString& id )
{
    setObjectName( id );
    ui.message->setText( message );

    style()->polish( this );
    style()->polish( ui.icon );

    QWidget::show();
}

void
MessageBar::onLinkActivated( const QString& link )
{
    // Show a dialog with the tracks
    ScrobbleConfirmationDialog confirmDialog( m_tracks );
    confirmDialog.setReadOnly();
    confirmDialog.exec();
}

