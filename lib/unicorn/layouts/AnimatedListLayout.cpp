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
#include "AnimatedListLayout.h"
#include <QWidget>
#include <QLayoutItem>
#include <QTimeLine>

AnimatedListLayout::AnimatedListLayout( QWidget* parent )
           : QLayout( parent ), m_timeLine( new QTimeLine( 1500, this ) )
{
    m_timeLine->setUpdateInterval( 20 );
    m_timeLine->setEasingCurve( QEasingCurve::OutBounce );

    connect( m_timeLine, SIGNAL(frameChanged( int )), SLOT(onFrameChanged( int )));
    connect( m_timeLine, SIGNAL(finished()), SIGNAL(moveFinished()));
}


AnimatedListLayout::~AnimatedListLayout()
{
    QLayoutItem *l;
    while ((l = takeAt(0)))
        delete l;
}


void 
AnimatedListLayout::addItem( QLayoutItem* item )
{    
    m_newItemList.push_back( item );

    connect( item->widget(), SIGNAL(loaded()), SLOT(onItemLoaded()));
}

void
AnimatedListLayout::onItemLoaded()
{
    int cumHeight(0);

    while ( m_newItemList.count() > 0 &&
            m_newItemList.front()->widget()->isVisible() )
    {
        QLayoutItem* item = m_newItemList.takeFirst();
        m_itemList.push_front( item );
        cumHeight += item->sizeHint().height();
    }

    if ( cumHeight > 0 )
    {
        m_timeLine->setDirection( QTimeLine::Forward );
        m_timeLine->setStartFrame( 0 + cumHeight + m_timeLine->currentFrame() );
        m_timeLine->setEndFrame( 0 );
        m_timeLine->start();
    }
}


Qt::Orientations 
AnimatedListLayout::expandingDirections() const
{
    return (Qt::Horizontal | Qt::Vertical);
}


bool 
AnimatedListLayout::hasHeightForWidth() const
{
    return false;
}


int
AnimatedListLayout::count() const
{
    return m_itemList.count();
}


QLayoutItem* 
AnimatedListLayout::itemAt( int index ) const
{
    if( index >= m_itemList.count() || index < 0 || m_itemList.isEmpty() )
        return 0;
    
    return m_itemList.at( index );
}


QSize 
AnimatedListLayout::minimumSize() const
{
    QSize minSize;
    
    foreach( QLayoutItem* i, m_itemList )
        minSize = minSize.expandedTo( i->minimumSize() );
    
    return minSize;
}


void 
AnimatedListLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry( rect );

    if( m_itemList.count() == 0 || m_timeLine->state() == QTimeLine::Running )
        return;
    
    doLayout( rect );
}


void 
AnimatedListLayout::doLayout( const QRect& rect, int vOffset )
{
    int cumHeight = 0;

    foreach( QLayoutItem* i, m_itemList )
    {
        QRect itemRect = rect;
        itemRect.setTop( rect.top() - vOffset + cumHeight );
        itemRect.setBottom( itemRect.top() + i->sizeHint().height() );
        i->setGeometry( itemRect );
        cumHeight += i->sizeHint().height();
    }

    QLayout::setGeometry( rect );
}


QSize 
AnimatedListLayout::sizeHint() const
{
    QSize sh( geometry().width() , 0);

    for ( int i(0) ; i < m_itemList.count() && i < 10 ; ++i )
        sh = sh += QSize( 0, m_itemList[i]->sizeHint().height() );
    
    return sh;
}


QLayoutItem* 
AnimatedListLayout::takeAt( int index )
{
    if( index >= m_itemList.count() || index < 0 || m_itemList.isEmpty() )
        return 0;
    
    return m_itemList.takeAt( index );
}


void 
AnimatedListLayout::onFrameChanged( int frame )
{
    doLayout( geometry(), frame );
}

