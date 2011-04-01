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
#include <QMouseEvent>

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


QWidget*
ActivityListItem::basicInfoWidget() const
{
    return infoWidget();
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
    movie.scrobbler_as->setSpeed( 200 );
    movie.scrobbler_as->setCacheMode( QMovie::CacheAll );
    ui.as->setMovie( movie.scrobbler_as );

    layout->addWidget( ui.as );

    ui.textArea = new QWidget( this );
    QHBoxLayout* h1 = new QHBoxLayout( ui.textArea );
    h1->setSpacing( 0 );
    h1->setContentsMargins( 0, 0, 0, 0);

    h1->addWidget( ui.text = new QLabel( "" ) );
    ui.text->setObjectName( "trackText" );

    h1->addWidget( ui.correction = new QLabel() );
    ui.correction->setObjectName( "correction" );
    ui.correction->hide();

    h1->addStretch( 1 );

    h1->addWidget( ui.love = new QLabel("love") );
    ui.love->setObjectName( "love" );
    ui.love->setToolTip( tr( "A loved track" ) );
    ui.love->hide();

    layout->addWidget( ui.textArea, 1 );

    ui.timestampArea = new QWidget( this );
    QHBoxLayout* h2 = new QHBoxLayout( ui.timestampArea );
    h2->setSpacing( 0 );
    h2->setContentsMargins( 0, 0, 0, 0);

    h2->addStretch( 0 );

    h2->addWidget( ui.timestamp = new QLabel(), 1 );
    ui.timestamp->setObjectName( "timestamp" );

    layout->addWidget( ui.timestampArea, 1 );
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
        m_timestampTimer->setSingleShot( true );
        connect( m_timestampTimer, SIGNAL(timeout()), SLOT(updateTimestamp()));
    }

    ui.as->clear();

    QDateTime now = QDateTime::currentDateTime();

    QString dateFormat( "d MMM h:mmap" );

    // Full time in the tool tip
    ui.timestamp->setToolTip(m_timestamp.toString( dateFormat ));

    int secondsAgo = m_timestamp.secsTo( now );

    if ( secondsAgo < (60 * 60) )
    {
        // Less than an hour ago
        int minutesAgo = ( m_timestamp.secsTo( now ) / 60 );
        m_timestampText = (minutesAgo == 1 ? tr( "%1 minute ago" ) : tr( "%1 minutes ago" ) ).arg( QString::number( minutesAgo ) );
        m_timestampTimer->start( now.secsTo( m_timestamp.addSecs(((minutesAgo + 1 ) * 60 ) + 1 ) ) * 1000 );
    }
    else if ( secondsAgo < (60 * 60 * 6) || now.date() == m_timestamp.date() )
    {
        // Less than 6 hours ago or on the same date
        int hoursAgo = ( m_timestamp.secsTo( now ) / (60 * 60) );
        m_timestampText = (hoursAgo == 1 ? tr( "%1 hour ago" ) : tr( "%1 hours ago" ) ).arg( QString::number( hoursAgo ) );
        m_timestampTimer->start( now.secsTo( m_timestamp.addSecs( ( (hoursAgo + 1) * 60 * 60 ) + 1 ) ) * 1000 );
    }
    else
    {
        m_timestampText = m_timestamp.toString( dateFormat );
        // We don't need to set the timer because this date will never change
    }

    // This makes it set the timestamp text
    resizeEvent(0);
}


void
ActivityListItem::mousePressEvent( QMouseEvent* event )
{
    if ( event->button() == Qt::LeftButton)
        emit clicked( this );
}


void
ActivityListItem::setText( const QString& text )
{
    m_text = text;
    resizeEvent(0);
}


void
ActivityListItem::resizeEvent(QResizeEvent* )
{
    int textAreaWidth;
    if( !ui.timestamp->text().isEmpty())
        textAreaWidth = ( width() * 65 ) / 100;
    else
        textAreaWidth = width();

    ui.textArea->setFixedWidth( textAreaWidth );

    if ( ui.correction->isVisible() )
        textAreaWidth -=  ui.correction->width();

    textAreaWidth -=  ui.love->width();

    QFontMetrics fm( ui.text->font() );
    ui.text->setText( fm.elidedText ( m_text, Qt::ElideRight, textAreaWidth ) );

    ui.timestamp->setText( m_timestampText );
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
