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

#include "ItemSelectorWidget.h"

#include <lastfm/User>

ItemSelectorWidget::ItemSelectorWidget( QWidget* parent )
    :StylableWidget(parent), m_clearText( false )
{
    QLayout* layout = new FlowLayout( this, 0, 0, 0 );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );
}

void
ItemSelectorWidget::setType( Type type )
{
    if (type == User)
        layout()->addItem( new QWidgetItem( ui.searchBox = new UserSearch( this ) ) );
    else
        layout()->addItem( new QWidgetItem( ui.searchBox = new TagSearch( this ) ) );

    ui.searchBox->setFrame( false );
    ui.searchBox->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );

    setFocusPolicy( Qt::StrongFocus );
    setFocusProxy( ui.searchBox );

    connect( ui.searchBox, SIGNAL(editingFinished()), SLOT(onItemSelected()) );
    connect( ui.searchBox->completer()->popup(), SIGNAL(clicked(QModelIndex)), SLOT(onItemSelected()));
    connect( ui.searchBox, SIGNAL(textChanged(QString)), SLOT(onTextChanged(QString)));

    connect( ui.searchBox, SIGNAL(commaPressed()), SLOT(onItemSelected()) );
    connect( ui.searchBox, SIGNAL(deletePressed()), SLOT(onDeletePressed()) );
}


void
ItemSelectorWidget::onTextChanged( const QString& /*text*/ )
{
    //QFontMetrics fm(font());

    //int completerWidth = ui.userSearch->completer()->popup()->sizeHint().width();
    //int textWidth = fm.width( text + "  " );

    //ui.userSearch->setFixedWidth( completerWidth > textWidth ? completerWidth : textWidth );
}

void
ItemSelectorWidget::onItemSelected()
{
    addItem( ui.searchBox->text() );
}

void
ItemSelectorWidget::onItemDeleted( QLabel* item )
{
    m_items.removeAt( m_items.indexOf( item ) );
    layout()->removeWidget( item );
    item->deleteLater();

    ui.searchBox->setFocus( Qt::OtherFocusReason );

    emit changed();
}

void
ItemSelectorWidget::onDeletePressed()
{
    if ( m_items.count() > 0 )
    {
        QLabel* lastLabel = m_items.takeLast();
        int cursorPos = lastLabel->text().length();
        ui.searchBox->setText( lastLabel->text() + ui.searchBox->text() );
        ui.searchBox->setCursorPosition( cursorPos );
        onItemDeleted( lastLabel );
    }
}

void
ItemSelectorWidget::onCompleterActivated( const QString& text )
{
    addItem( text );
}

void
ItemSelectorWidget::addItem( const QString& text )
{
    if ( !ui.searchBox->text().isEmpty() // don't add empty recipients
        && !itemsContain( text ) // don't add duplicates
        && m_items.count() < 10 ) // limit to 10
    {
        QLabel* item = new QLabel( text, this );
        m_items.append( item );
        dynamic_cast<FlowLayout*>(layout())->insertWidget( layout()->count() - 1 , item );

        // clear the line edit a little bit later because the QCompleter
        // will set the text to be what was selected after this
        QTimer::singleShot(1, ui.searchBox, SLOT(clear()));

        emit changed();
    }
}

bool
ItemSelectorWidget::itemsContain( const QString& text )
{
    foreach ( const QLabel* item, m_items )
    {
        if ( item->text() == text )
            return true;
    }

    return false;
}

QStringList
ItemSelectorWidget::items() const
{
    QStringList items;

    foreach (const QLabel* item, m_items)
    {
        items << item->text();
    }

    return items;
}

