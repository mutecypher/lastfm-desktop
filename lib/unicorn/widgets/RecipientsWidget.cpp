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
#include <QLabel>
#include <QComboBox>
#include <QCompleter>
#include <QAbstractItemView>
#include <QTimer>

#include "lib/unicorn/widgets/SearchBox.h"

#include "lib/unicorn/layouts/flowlayout.h"

#include "RecipientsWidget.h"
#include "RecipientWidget.h"

RecipientsWidget::RecipientsWidget(QWidget* parent)
    :StylableWidget(parent), m_clearText( false )
{
    new FlowLayout( this, 0, 0, 0 );

    layout()->setContentsMargins( 1, 1, 1, 1 );

    layout()->addItem( new QWidgetItem( ui.userSearch = new UserSearch( this ) ) );
    ui.userSearch->setFrame( false );
    setFocusPolicy( Qt::StrongFocus );
    setFocusProxy( ui.userSearch );

    connect( ui.userSearch, SIGNAL(editingFinished()), SLOT(onRecipientSelected()) );
    connect( ui.userSearch->completer()->popup(), SIGNAL(clicked(QModelIndex)), SLOT(onRecipientSelected()));
    connect( ui.userSearch, SIGNAL(textChanged(QString)), SLOT(onTextChanged(QString)));

    connect( ui.userSearch, SIGNAL(commaPressed()), SLOT(onRecipientSelected()) );
    connect( ui.userSearch, SIGNAL(deletePressed()), SLOT(onDeletePressed()) );
}

void
RecipientsWidget::onTextChanged( const QString& text )
{
    //QFontMetrics fm(font());

    //int completerWidth = ui.userSearch->completer()->popup()->sizeHint().width();
    //int textWidth = fm.width( text + "  " );

    //ui.userSearch->setFixedWidth( completerWidth > textWidth ? completerWidth : textWidth );
}

void
RecipientsWidget::onRecipientSelected()
{
    addRecipient( ui.userSearch->text() );
}

void
RecipientsWidget::onRecipientDeleted( RecipientWidget* recipient )
{
    m_recipients.removeAt( m_recipients.indexOf( recipient ) );
    layout()->removeWidget( recipient );
    recipient->deleteLater();

    ui.userSearch->setFocus( Qt::OtherFocusReason );
}

void
RecipientsWidget::onDeletePressed()
{
    if ( m_recipients.count() > 0 )
    {
        onRecipientDeleted( m_recipients.takeLast() );
    }
}

void
RecipientsWidget::onCompleterActivated( const QString& text )
{
    addRecipient( text );
}

void
RecipientsWidget::addRecipient( const QString& text )
{
    // create the widget
    RecipientWidget* recipient = new RecipientWidget( text, this );

    if ( !ui.userSearch->text().isEmpty() // don't add empty recipients
        && !recipientsContain( text ) // don't add duplicates
        && m_recipients.count() < 10 ) // limit to 10
    {
        // make sure we get told when the widget is deleted
        connect( recipient, SIGNAL(deleted(RecipientWidget*)), SLOT(onRecipientDeleted(RecipientWidget*)) );

        m_recipients.append( recipient );
        dynamic_cast<FlowLayout*>(layout())->insertWidget( layout()->count() - 1 , recipient );

        // clear the line edit a little bit later because the QCompleter
        // will set the text to be what was selected after this
        QTimer::singleShot(1, ui.userSearch, SLOT(clear()));

        emit changed();
    }
    else
    {
        recipient->deleteLater();
    }
}

bool
RecipientsWidget::recipientsContain( const QString& text )
{
    foreach (const RecipientWidget* recipient, m_recipients)
    {
        if ( recipient->recipient() == text )
            return true;
    }

    return false;
}

QStringList
RecipientsWidget::recipients() const
{
    QStringList recipients;

    foreach (const RecipientWidget* recipient, m_recipients)
    {
        recipients << recipient->recipient();
    }

    return recipients;
}

