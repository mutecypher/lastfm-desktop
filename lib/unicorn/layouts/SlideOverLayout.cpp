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
#include "SlideOverLayout.h"
#include <QWidget>
#include <QLayoutItem>
#include <QTimeLine>
#include <QDebug>

SlideOverLayout::SlideOverLayout( QWidget* parent )
           : QLayout( parent ), m_currentItem( 0 ), m_previousItem( 0 ), m_timeLine( new QTimeLine( 200, this ) )
{
    m_timeLine->setUpdateInterval( 20 );
    m_timeLine->setCurveShape( QTimeLine::EaseInCurve );
    connect( m_timeLine, SIGNAL(frameChanged( int )), SLOT(onFrameChanged( int )));
    connect( m_timeLine, SIGNAL(finished()), SLOT(onMoveFinished()));
}


SlideOverLayout::~SlideOverLayout()
{
    while( QLayoutItem* i = takeAt( 0 ) )
        delete i;
}


void
SlideOverLayout::onMoveFinished()
{
    if( m_currentItem != m_itemList[0] )
        m_itemList[0]->widget()->hide();

    emit moveFinished( m_currentItem );
}


void 
SlideOverLayout::addItem(QLayoutItem *item)
{
    m_itemList.push_back( item );
    if( !m_currentItem )
        m_currentItem = item;
}


Qt::Orientations 
SlideOverLayout::expandingDirections() const
{
    return (Qt::Horizontal | Qt::Vertical);
}


bool 
SlideOverLayout::hasHeightForWidth() const
{
    return false;
}


int
SlideOverLayout::count() const
{
    return m_itemList.count();
}


QLayoutItem* 
SlideOverLayout::itemAt( int index ) const
{
    if( index >= m_itemList.count() || index < 0 || m_itemList.isEmpty() )
        return 0;
    
    return m_itemList.at( index );
}


QSize 
SlideOverLayout::minimumSize() const
{
    QSize minSize;
    
    foreach( QLayoutItem* i, m_itemList )
        minSize = minSize.expandedTo( i->minimumSize() );
    
    return minSize;
}


void 
SlideOverLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry( rect );
    if( !m_currentItem || m_timeLine->state() == QTimeLine::Running )
        return;
    
    doLayout( rect );
}


void 
SlideOverLayout::doLayout( const QRect& rect, int vOffset )
{
    if( m_itemList.isEmpty())
        return;

    m_itemList[0]->setGeometry( rect );

    foreach( QLayoutItem* i, m_itemList )
    {
        if( i == m_itemList[0] ) continue;

        if( i == m_currentItem ) {
            if( vOffset > -1 ) {
                i->setGeometry( rect.translated( 0, vOffset ));
            } else
                i->setGeometry( rect.translated( 0, 0 ) );

        } else {
            if( i == m_previousItem && m_currentItem == m_itemList[ 0 ] && vOffset > -1 ) {
                i->setGeometry( rect.translated( 0, vOffset ));
            } else
                i->setGeometry( rect.translated( 0, rect.height()));
        }
    }
}


QSize 
SlideOverLayout::sizeHint() const
{
    QSize sh;
    foreach( QLayoutItem* i, m_itemList )
        sh = sh.expandedTo( i->sizeHint() );
    
    return sh;
}


QLayoutItem* 
SlideOverLayout::takeAt( int index )
{
    if( index >= m_itemList.count() || index < 0 || m_itemList.isEmpty() )
        return 0;
    
    return m_itemList.takeAt( index );
}

/*
void
SlideOverLayout::moveForward()
{
    int nextIndex = m_itemList.indexOf( m_currentItem ) + 1;
    if( nextIndex >= m_itemList.count() )
        return;

    QLayoutItem* previousItem = m_currentItem;
    m_currentItem = m_itemList.at( nextIndex );
    emit moveStarted( m_currentItem, previousItem );
    
    if( m_timeLine->state() == QTimeLine::Running && 
        m_timeLine->direction() == QTimeLine::Backward )
    {
        m_timeLine->setDirection( QTimeLine::Forward );
    }
    else
    {
        m_timeLine->setDirection( QTimeLine::Forward );
        m_timeLine->setStartFrame( geometry().width() );
        m_timeLine->setEndFrame( 0 );
        m_timeLine->start();
    }
 
}
*/

void
SlideOverLayout::revealWidget( QWidget* w )
{
    int index = -1;
    foreach( QLayoutItem* i, m_itemList ) 
    {
        if( i->widget() == w ) {
            index = m_itemList.indexOf( i );
            break;
        }
    }

    if( index == -1 ) return;

    m_previousItem = m_currentItem;

    m_currentItem = m_itemList[ index ];

    if( m_itemList[ index ] == m_itemList[ 0 ])
    {
        m_timeLine->stop();
        m_itemList[ 0 ]->widget()->show();
        m_timeLine->setStartFrame( 0 );
        m_timeLine->setEndFrame( geometry().height());
        m_timeLine->start();
        return;
    }
    m_timeLine->stop();
    m_timeLine->setDirection( QTimeLine::Forward );
    m_timeLine->setStartFrame( geometry().height());
    m_timeLine->setEndFrame( 0 );
    m_timeLine->start();
}


void 
SlideOverLayout::onFrameChanged( int frame )
{
    doLayout( geometry(), frame );
}


QWidget* 
SlideOverLayout::currentWidget()
{
    return m_currentItem->widget();
}

QWidget* 
SlideOverLayout::prevWidget()
{
    int prevIndex = m_itemList.indexOf( m_currentItem ) - 1;
    return prevIndex >= 0 ? m_itemList[prevIndex]->widget() : 0;
}

QWidget* 
SlideOverLayout::nextWidget()
{
    int nextIndex = m_itemList.indexOf( m_currentItem ) + 1;
    return nextIndex < m_itemList.count() ? m_itemList[nextIndex]->widget() : 0;
}

