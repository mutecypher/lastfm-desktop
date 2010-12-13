/*
   Copyright 2010 Last.fm Ltd.
      - Primarily authored by Jono Cole and Michael Coffey

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

#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QMenu>
#include <QTimer>

#include "lib/unicorn/widgets/GhostWidget.h"

#include "../Application.h"
#include "ActivityListItem.h"
#include "IPodScrobbleItem.h"
#include "TrackItem.h"

ActivityListItem::ActivityListItem( QWidget* parent )
    :StylableWidget( parent ), m_timestampTimer( 0 ), m_selected( false )
{
}

ActivityListItem*
ActivityListItem::fromElement( QDomElement element )
{
    ActivityListItem* item(0);

    if ( element.attributes().contains("iPodScrobble") )
        item = new IPodScrobbleItem( element );
    else
        item = new TrackItem( element );

    return item;
}

void
ActivityListItem::setupUi()
{
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );

    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    ui.as = new QLabel();
    ui.as->setObjectName( "as" );

    movie.scrobbler_as = new Movie( ":/scrobbler_as.mng" );
    movie.scrobbler_paused = new Movie( ":/scrobbler_as_paused.mng" );
    movie.scrobbler_as->setCacheMode( QMovie::CacheAll );
    movie.scrobbler_paused->setCacheMode( QMovie::CacheAll );
    ui.as->setMovie( movie.scrobbler_as );

    layout->addWidget( ui.as );

    ui.trackTextArea = new QWidget( this );
    QHBoxLayout* h1 = new QHBoxLayout( ui.trackTextArea );
    h1->setSpacing( 0 );
    h1->setContentsMargins( 0, 0, 0, 0);

    h1->addWidget( ui.trackText = new QLabel( "" ) );
    ui.trackText->setObjectName( "trackText" );

    h1->addWidget( ui.correction = new QLabel() );
    ui.correction->setObjectName( "correction" );
    ui.correction->hide();

    h1->addStretch( 1 );

    layout->addWidget( ui.trackTextArea, 1 );

    layout->addWidget( ui.love = new QLabel("love") );
    ui.love->setObjectName( "love" );
    ui.love->hide();

    layout->addWidget( ui.timestamp = new QLabel() );
    ui.timestamp->setObjectName( "timestamp" );
}

QDomElement
ActivityListItem::toDomElement( QDomDocument xml ) const
{
    // The default behaviour is to do nothing
    return QDomElement();
}

void
ActivityListItem::updateTimestamp()
{
    if (!m_timestampTimer)
    {
        m_timestampTimer = new QTimer( this );
        connect( m_timestampTimer, SIGNAL(timeout()), SLOT(updateTimestamp()));
    }

    ui.as->clear();

    QDateTime now = QDateTime::currentDateTime();

    // Full time in the tool tip
    ui.timestamp->setToolTip(m_timestamp.toString( "ddd h:ssap" ));

    if ( m_timestamp.daysTo( now ) > 1 )
    {
        ui.timestamp->setText(m_timestamp.toString( "ddd h:ssap" ));
        m_timestampTimer->start( 24 * 60 * 60 * 1000 );
    }
    else if ( m_timestamp.daysTo( now ) == 1 )
    {
        ui.timestamp->setText( "Yesterday " + m_timestamp.toString( "h:ssap" ));
        m_timestampTimer->start( 24 * 60 * 60 * 1000 );
    }
    else if ( (m_timestamp.secsTo( now ) / (60 * 60) ) > 1 )
    {
        ui.timestamp->setText( QString::number( (m_timestamp.secsTo( now ) / (60 * 60) ) ) + " hours ago" );
        m_timestampTimer->start( 60 * 60 * 1000 );
    }
    else if ( (m_timestamp.secsTo( now ) / (60 * 60) ) == 1 )
    {
        ui.timestamp->setText( "1 hour ago" );
        m_timestampTimer->start( 60 * 60 * 1000 );
    }
    else if ( (m_timestamp.secsTo( now ) / 60 ) == 1 )
    {
        ui.timestamp->setText( "1 minute ago" );
        m_timestampTimer->start( 60 * 1000 );
    }
    else
    {
        ui.timestamp->setText( QString::number( (m_timestamp.secsTo( now ) / 60 ) ) + " minutes ago" );
        m_timestampTimer->start( 60 * 1000 );
    }
}


void
ActivityListItem::mousePressEvent( QMouseEvent * event )
{
    emit clicked( this );
}


void
ActivityListItem::resizeEvent(QResizeEvent* )
{
///    TODO:
//    int width =  ui.trackTextArea->width();
//    if (ui.correction->isVisible() ) width -=  ui.correction->width();

//    QFontMetrics fm( ui.trackText->font() );
//    ui.trackText->setText( fm.elidedText ( m_track.toString(), Qt::ElideRight, width) );
}


bool
ActivityListItem::odd() const
{
    return m_odd;
}


void
ActivityListItem::setOdd( bool odd )
{
    m_odd = odd;
}


bool
ActivityListItem::selected() const
{
    return m_selected;
}


void
ActivityListItem::setSelected( bool selceted )
{
    m_selected = selceted;
    setStyle(QApplication::style());
}


QString ActivityListItem::status() const
{
    return m_status;
}


void ActivityListItem::setStatus( QString status )
{
    m_status = status;
}
