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

#include <QDebug>
#include <QWidget>
#include <QLayoutItem>
#include <QTimeLine>

AnimatedListLayout::AnimatedListLayout( int size, QWidget* parent )
           : QLayout( parent ),
           m_timeLine( new QTimeLine( 1500, this ) ),
           m_animated( true ),
           m_size( size )
{
    m_timeLine->setUpdateInterval( 20 );

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
AnimatedListLayout::setEasingCurve( QEasingCurve::Type easingCurve )
{
    m_timeLine->setEasingCurve( easingCurve );
}

void
AnimatedListLayout::setAnimated( bool animated )
{
    m_animated = animated;

    if ( !m_animated )
    {
        // we are no longer animating so stop any current animation
        // and move all the items over

        m_timeLine->stop();

        while ( m_newItemList.count() != 0 )
            m_itemList.prepend( m_newItemList.takeLast() );

        doLayout( geometry() );
    }
}


void 
AnimatedListLayout::addItem( QLayoutItem* item )
{    
    if ( m_animated )
    {
        m_newItemList.prepend( item );

        // make sure we don't draw the item in the list until
        // it has been added properly
        QRect itemRect = contentsRect();
        itemRect.setHeight( 2 );
        itemRect.translate( -3, -3);
        item->setGeometry( itemRect );
    }
    else
    {
        m_itemList.prepend( item );
    }

    connect( item->widget(), SIGNAL(loaded()), SLOT(onItemLoaded()));
}

void
AnimatedListLayout::onItemLoaded()
{
    int cumHeight(0);
    bool allItemsLoaded = true;

    // check to see if all the new items are loaded yet
    foreach ( QLayoutItem* item , m_newItemList )
    {
        if ( item->widget()->isHidden() )
        {
            allItemsLoaded = false;
            break;
        }
    }

    if ( allItemsLoaded )
    {
        // All the items have loaded so calculate how tall
        // they are and prepend them to the proper item list

        foreach ( QLayoutItem* item , m_newItemList )
        {
            m_itemList.prepend( item );
            cumHeight += item->sizeHint().height();
        }

        m_newItemList.clear();
    }

    if ( cumHeight > 0 )
    {
        // Start the animation!
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
    return m_newItemList.count() + m_itemList.count();
}


QLayoutItem* 
AnimatedListLayout::itemAt( int index ) const
{
    int totalCount = m_newItemList.count() + m_itemList.count();

    if( index >= totalCount || index < 0 || totalCount == 0 )
        return 0;
    
    if ( index < m_newItemList.count() )
        return m_newItemList.at( index );

    return m_itemList.at( index - m_newItemList.count() );
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

    // make sure that all the unloaded items are off the screen
    foreach( QLayoutItem* i, m_newItemList )
    {
        QRect itemRect = rect;
        itemRect.moveLeft( itemRect.width() * 2 );
        i->setGeometry( itemRect );
    }

    // Draw the items in order according to the animation
    // vOffset is where we are in the animation
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

    for ( int i(0) ; i < count() && i < m_size ; ++i )
        sh = sh += QSize( 0, itemAt(i)->sizeHint().height() );
    
    return sh;
}


QLayoutItem* 
AnimatedListLayout::takeAt( int index )
{
    int totalCount = m_newItemList.count() + m_itemList.count();

    if( index >= totalCount || index < 0 || totalCount == 0 )
        return 0;

    if ( index < m_newItemList.count() )
        return m_newItemList.takeAt( index );

    return m_itemList.takeAt( index - m_newItemList.count());
}


void 
AnimatedListLayout::onFrameChanged( int frame )
{
    doLayout( geometry(), frame );
}

