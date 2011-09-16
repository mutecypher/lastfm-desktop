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
#include "MessageBar.h"
#include "common/qt/reverse.cpp"
#include <QtGui>


MessageBar::MessageBar( QWidget* parent )
           :QWidget( parent )
{
    setFixedHeight( 0 );

    ui.papyrus = new QWidget( this );
    
    m_timeline = new QTimeLine( 500, this );
    m_timeline->setUpdateInterval( 10 );

    connect( m_timeline, SIGNAL(frameChanged( int )), SLOT(animate( int )) );
}


void
MessageBar::show( const QString& message, const QString& id )
{    
    QLabel* label = new QLabel( message, ui.papyrus );
    label->setBackgroundRole( QPalette::Base );
    label->setMargin( 8 );
    label->setIndent( 4 );
    label->setTextFormat( Qt::RichText );
    label->setOpenExternalLinks( true );
    label->setTextInteractionFlags( Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse );
    label->setObjectName( id );

    label->adjustSize();

    show( label );
}


void 
MessageBar::show( QWidget* w )
{
    QPushButton* close = new QPushButton( "x" );
    QHBoxLayout* h = new QHBoxLayout( w );
    h->addStretch();

    h->addWidget( close );
    h->setMargin( 4 );
    
    ui.papyrus->move( 0, -w->height() );

    w->setFixedWidth( width() );
    w->setParent( this );
    w->show();

    doLayout();
    
    connect( close, SIGNAL(clicked()), w, SLOT(deleteLater()) );    
    connect( w, SIGNAL(destroyed()), SLOT(onLabelDestroyed()), Qt::QueuedConnection );
        
    m_timeline->setFrameRange( height(), ui.papyrus->height() );
    m_timeline->start();
}


void
MessageBar::animate( int i )
{
    setFixedHeight( i );
    ui.papyrus->move( 0, i - ui.papyrus->height() );
}


void
MessageBar::doLayout()
{
    int y = 0;
    foreach (QLabel* l, Qt::reverse<QLabel*>( findChildren<QLabel*>() ))
    {        
        l->move( 0, y );
        y += l->height();
    }
    ui.papyrus->setFixedSize( width(), y );
}


void
MessageBar::onLabelDestroyed()
{
    doLayout();
    setFixedHeight( ui.papyrus->height() );
}

void
MessageBar::remove( const QString& id )
{
    delete findChild<QLabel*>( id );
}

