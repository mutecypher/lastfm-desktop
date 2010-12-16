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

#include <QLabel>

#include <lastfm/ws.h>

#include "ScrobbleInfoWidget.h"
#include "IPodScrobbleItem.h"


IPodScrobbleItem::IPodScrobbleItem( const QList<Track>& tracks )
    :ActivityListItem(), m_scrobbleText( 0 )
{
    setupUi();

    QString format = (tracks.count() == 1 ?
        tr("%1 track from the iPod '%2'"):
        tr("%1 tracks from the iPod '%2'"));

    setText( format.arg( QString::number(tracks.count()), tracks[0].extra("deviceId") ) );

    m_timestamp = QDateTime::currentDateTime();
    updateTimestamp();
}

IPodScrobbleItem::IPodScrobbleItem( const QDomElement& element )
    :ActivityListItem(), m_scrobbleText( 0 )
{
    setupUi();

    setText( element.firstChildElement("text").text() );
    m_timestamp.setTime_t( element.firstChildElement("timestamp").text().toUInt() );
    updateTimestamp();
}


QWidget*
IPodScrobbleItem::infoWidget() const
{
    if ( !m_scrobbleText )
        m_scrobbleText = new QLabel( m_text );

    return m_scrobbleText;
}


QDomElement
IPodScrobbleItem::toDomElement( QDomDocument xml ) const
{
    QDomElement element = xml.createElement( "track" );
    element.setAttribute("iPodScrobble", "1");

    QDomElement trackText = xml.createElement( "text" );
    trackText.appendChild( xml.createTextNode( m_text ) );
    element.appendChild( trackText );

    QDomElement timestampText = xml.createElement( "timestamp" );
    timestampText.appendChild( xml.createTextNode( QString::number( m_timestamp.toTime_t() ) ) );
    element.appendChild( timestampText );

    return element;
}
