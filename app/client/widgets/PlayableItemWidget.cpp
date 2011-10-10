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

#include <lastfm/RadioStation.h>

#include "PlayableItemWidget.h"
#include "../Services/RadioService.h"

PlayableItemWidget::PlayableItemWidget( QWidget* parent )
    : QPushButton( parent ),
      m_hovered( false ),
      m_radio_left_hover( ":/meta_radio_LEFT_HOVER.png" ),
      m_radio_left_press( ":/meta_radio_LEFT_PRESS.png" ),
      m_radio_left_rest( ":/meta_radio_LEFT_REST.png" ),
      m_radio_middle_hover( ":/meta_radio_MIDDLE_HOVER.png" ),
      m_radio_middle_press( ":/meta_radio_MIDDLE_PRESS.png" ),
      m_radio_middle_rest( ":/meta_radio_MIDDLE_REST.png" ),
      m_radio_right_hover( ":/meta_radio_RIGHT_HOVER.png" ),
      m_radio_right_press( ":/meta_radio_RIGHT_PRESS.png" ),
      m_radio_right_rest( ":/meta_radio_RIGHT_REST.png" )
{
    setAttribute( Qt::WA_LayoutUsesWidgetRect );
    setCursor( Qt::PointingHandCursor );
    setAttribute( Qt::WA_Hover );
}

PlayableItemWidget::PlayableItemWidget( const RadioStation& rs, const QString& title, const QString& description, QWidget* parent )
    : QPushButton( parent ), m_rs(rs), m_description( description ), m_hovered( false )
{
    setStation( rs, title, description );

    setAttribute( Qt::WA_LayoutUsesWidgetRect );
    setCursor( Qt::PointingHandCursor );
}

bool
PlayableItemWidget::event( QEvent* e )
{
    switch ( e->type() )
    {
    case QEvent::HoverEnter:
        m_hovered = true;
        update();
        break;
    case QEvent::HoverLeave:
        m_hovered = false;
        update();
        break;
    default:
        break;
    }

    return QPushButton::event( e );
}

void
PlayableItemWidget::setStation(const RadioStation& rs, const QString& title, const QString& description)
{
    // disconnect from recieving any previous signals
    disconnect( this, 0 );

    m_rs = rs;
    m_rs.setTitle( title );
    setText( title );

    m_description = description;

    connect( &RadioService::instance(), SIGNAL(tuningIn(RadioStation)), SLOT(onRadioChanged()) );
    connect( &RadioService::instance(), SIGNAL(trackSpooled(Track)), SLOT(onRadioChanged()));

    connect( this, SIGNAL(clicked()), SLOT(play()));

    update();
}

void
PlayableItemWidget::setDescription( const QString& description )
{
    m_description = description;
    update();
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

    RadioStation station = RadioStation::library( users );
    station.setTitle( tr( "Multi-Library Radio" ) );
    return station;
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
    if ( QString("ProfileWidget").compare( parent()->parent()->parent()->metaObject()->className() ) == 0 )
    {
        QPushButton::paintEvent( event );

        QPainter p( this );

        p.setPen( QColor( 0x898989 ) );

        QFont font = p.font();
        font.setPixelSize( 12 );
        p.setFont( font );

        QTextOption to;
        to.setAlignment( Qt::AlignVCenter );

        QFontMetrics fm( font );
        p.drawText( rect().adjusted( fm.width( text() ) + 45, 0, 0, 0 ), m_description, to );

    }
    else if ( QString("MetadataWidget").compare( parent()->parent()->parent()->parent()->parent()->metaObject()->className() ) == 0 )
    {
        QPainter p( this );

        QRect middleRect = QRect( m_radio_left_rest.width(), 0, rect().width() - m_radio_left_rest.width() - m_radio_right_rest.width(), 49 );

        if ( isDown() )
        {
            p.drawPixmap( rect().topLeft(), m_radio_left_press );
            p.drawPixmap( middleRect, m_radio_middle_press );
            p.drawPixmap( rect().topLeft() + QPoint( rect().width() - m_radio_right_press.width(), 0 ), m_radio_right_press );
        }
        else if ( m_hovered )
        {
            p.drawPixmap( rect().topLeft(), m_radio_left_hover );
            p.drawPixmap( middleRect, m_radio_middle_hover );
            p.drawPixmap( rect().topLeft() + QPoint( rect().width() - m_radio_right_hover.width(), 0 ), m_radio_right_hover );
        }
        else
        {
            p.drawPixmap( rect().topLeft(), m_radio_left_rest );
            p.drawPixmap( middleRect, m_radio_middle_rest );
            p.drawPixmap( rect().topLeft() + QPoint( rect().width() - m_radio_right_rest.width(), 0 ), m_radio_right_rest );
        }

        p.drawText( rect().adjusted( 40, 8, 0, 0 ), text() );

        p.setPen( QColor( 0x898989 ) );

        QFont font = p.font();
        font.setPixelSize( 10 );
        p.setFont( font );

        p.drawText( rect().adjusted( 40, 28, 0, 0 ), m_description );
    }
    else
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
