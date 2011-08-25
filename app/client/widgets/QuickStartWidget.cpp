/*
   Copyright 2005-2009 Last.fm Ltd. 

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

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QPoint>
#include <QMenu>
#include <QShortcut>

#include "QuickStartWidget.h"
#include "../StationSearch.h"
#include "../Services/RadioService/RadioService.h"

#include <QStylePainter>

QuickStartWidget::QuickStartWidget( QWidget* parent )
    :StylableWidget( parent )
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    layout->addWidget( ui.edit = new QLineEdit( this ) );
    ui.edit->setPlaceholderText( tr("Type an artist or tag and press play") );
    ui.edit->setAttribute( Qt::WA_MacShowFocusRect, false );

    connect( ui.edit, SIGNAL(returnPressed()), SLOT(play()));

    layout->addWidget( ui.button = new QPushButton( tr("Play"), this ) );
    ui.button->setToolTip( tr( "Play" ) );
    ui.button->setContextMenuPolicy( Qt::CustomContextMenu );
    ui.button->setEnabled( false );

    connect( ui.button, SIGNAL(clicked()), SLOT(play()));
    connect( ui.button, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customContextMenuRequested(QPoint)));

    connect( ui.edit, SIGNAL(textChanged(QString)), SLOT(onTextChanged(QString)));

    QShortcut* shortcut = new QShortcut( ui.edit );
    shortcut->setKey( Qt::CTRL + Qt::Key_D );
    shortcut->setContext( Qt::WidgetShortcut );
    connect( shortcut, SIGNAL(activated()), SLOT(setToCurrent()) );
}

void
QuickStartWidget::setToCurrent()
{
    ui.edit->setText( RadioService::instance().station().url() );
}

void
QuickStartWidget::onTextChanged( const QString& text )
{
    ui.button->setEnabled( !text.isEmpty() );
}

void
QuickStartWidget::play()
{
    QString trimmedText = ui.edit->text().trimmed();

    if( trimmedText.startsWith("lastfm://") )
        RadioService::instance().play( RadioStation( trimmedText ) );
    else if ( ui.edit->text().length() )
    {
        StationSearch* s = new StationSearch();
        connect(s, SIGNAL(searchResult(RadioStation)), &RadioService::instance(), SLOT(play(RadioStation)));
        s->startSearch( ui.edit->text() );
    }

    ui.edit->clear();
}

void
QuickStartWidget::playNext()
{
    QString trimmedText = ui.edit->text().trimmed();

    if( trimmedText.startsWith("lastfm://"))
        RadioService::instance().playNext( RadioStation( trimmedText ) );
    else if ( ui.edit->text().length() )
    {
        StationSearch* s = new StationSearch();
        connect(s, SIGNAL(searchResult(RadioStation)), &RadioService::instance(), SLOT(playNext(RadioStation)));
        s->startSearch(ui.edit->text());
    }

    ui.edit->clear();
}

void
QuickStartWidget::customContextMenuRequested( const QPoint& point )
{
    QMenu* contextMenu = new QMenu( this );

    if ( !ui.edit->text().isEmpty() )
    {
        contextMenu->addAction( tr( "Play" ), this, SLOT(play()));

        if ( RadioService::instance().state() == Playing )
            contextMenu->addAction( tr( "Play next" ), this, SLOT(playNext()));
    }

    if ( contextMenu->actions().count() )
        contextMenu->exec( ui.button->mapToGlobal( point ) );
}
