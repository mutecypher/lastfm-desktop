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

#include <QPainter>
#include <QPaintEvent>
#include <QLayout>
#include <QLabel>
#include <QStyle>
#include <QFont>
#include <QMenu>

#include <lastfm/RadioStation>

#include "PlayableItemWidget.h"
#include "../Services/RadioService.h"


PlayableItemWidget::PlayableItemWidget(const RadioStation& rs, QString title, QString description )
    : m_rs(rs), m_description( description )
{
    m_rs.setTitle( title );
    setText( title );

    setAttribute( Qt::WA_LayoutUsesWidgetRect );

    connect( &RadioService::instance(), SIGNAL(tuningIn(RadioStation)), SLOT(onRadioChanged()) );
    connect( &RadioService::instance(), SIGNAL(trackSpooled(Track)), SLOT(onRadioChanged()));

    connect( this, SIGNAL(clicked()), SLOT(play()));
}


void 
PlayableItemWidget::play()
{
    RadioService::instance().play( m_rs );
}

void
PlayableItemWidget::playNext()
{
    RadioService::instance().playNext( m_rs );
}

RadioStation
PlayableItemWidget::getMultiStation() const
{
    QList<User> users;

    int endPos = m_rs.url().indexOf( "/", 14 );
    if ( endPos == -1 )
        endPos = m_rs.url().length();

    users << User( m_rs.url().mid( 14, endPos - 14 ) );
    users << User();

    return RadioStation::library( users );
}

void
PlayableItemWidget::playMulti()
{
    if ( m_rs.url().startsWith("lastfm://user/") )
        RadioService::instance().play( getMultiStation() );
}

void
PlayableItemWidget::playMultiNext()
{
    if ( m_rs.url().startsWith("lastfm://user/") )
        RadioService::instance().playNext( getMultiStation() );
}


void
PlayableItemWidget::onRadioChanged()
{
    if ( RadioService::instance().station() == m_rs )
        setText( RadioService::instance().station().title() );
}

void
PlayableItemWidget::paintEvent( QPaintEvent* event )
{
    QPushButton::paintEvent( event );
    QPainter p( this );

    p.setPen( QColor( 0x898989 ) );

    QFont font = p.font();
    font.setPixelSize( 12 );
    p.setFont( font );

    QTextOption to;
    to.setAlignment( Qt::AlignBottom );

    QRect rect = contentsRect();
    rect.adjust( 54, 0, 0, -14 );

    p.drawText( rect, m_description, to );
}

void
PlayableItemWidget::contextMenuEvent( QContextMenuEvent* event )
{
    QMenu* contextMenu = new QMenu( this );

    contextMenu->addAction( tr( "Play" ), this, SLOT(play()));

    if ( RadioService::instance().state() == Playing )
        contextMenu->addAction( tr( "Play next" ), this, SLOT(playNext()));

    if ( m_rs.url().startsWith( "lastfm://user/" )
         &&  ( m_rs.url().endsWith( "/library" ) || m_rs.url().endsWith( "/personal" ) )
         && m_rs.url() != RadioStation::library( User() ).url() )
    {
        // if it's a user station that isn't yours we should
        // let them start a multi-station with yours
        contextMenu->addSeparator();
        contextMenu->addAction( tr( "Play with your library" ), this, SLOT(playMulti()));
        if ( RadioService::instance().state() == Playing )
            contextMenu->addAction( tr( "Play with your library next" ), this, SLOT(playMultiNext()));
    }

    if ( contextMenu->actions().count() )
        contextMenu->exec( event->globalPos() );
}
