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

#include "PlayableItemWidget.h"
#include "../Services/RadioService.h"


PlayableItemWidget::PlayableItemWidget(const RadioStation& rs, QString title, QString description )
    : m_rs(rs), m_description( description )
{
    m_rs.setTitle( title );
    init();
}


void
PlayableItemWidget::init()
{
    QString title = fontMetrics().elidedText( m_rs.title(), Qt::ElideRight, 200 );
    if( title != m_rs.title() )
        setToolTip( m_rs.title());
    setText( title );

    connect( this, SIGNAL(startRadio(RadioStation)), &RadioService::instance(), SLOT(play(RadioStation)) );

    connect( &RadioService::instance(), SIGNAL(tuningIn(RadioStation)), SLOT(onRadioChanged()) );
    connect( &RadioService::instance(), SIGNAL(trackSpooled(Track)), SLOT(onRadioChanged()));

    connect( this, SIGNAL(clicked()), SLOT(onClicked()));
}


void 
PlayableItemWidget::onClicked()
{
    emit startRadio( m_rs );
}


void
PlayableItemWidget::onRadioChanged()
{
    if ( RadioService::instance().station() == m_rs )
    {
        setEnabled( false );
        setText( RadioService::instance().station().title() );
    }
    else
        setEnabled( true );

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
