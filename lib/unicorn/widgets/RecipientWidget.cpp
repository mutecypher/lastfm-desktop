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

#include <QDebug>
#include <QHBoxLayout>

#include "RecipientWidget.h"

RecipientWidget::RecipientWidget( const QString& recipient, QWidget* parent )
    :StylableWidget(parent)
{
    new QHBoxLayout( this );

    layout()->setContentsMargins( 0, 0, 0, 0 );
    layout()->addWidget( ui.recipient = new QLabel( recipient, this ) );
    layout()->addWidget( ui.deleteButton = new QPushButton( tr("delete"), this ) );

    connect( ui.deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteClicked()));
}

QString
RecipientWidget::recipient() const
{
    return ui.recipient->text();
}

void
RecipientWidget::onDeleteClicked()
{
    emit deleted( this );
}

