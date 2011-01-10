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
#include "IPodScrobbleInfoWidget.h"
#include "IPodScrobbleItem.h"


IPodScrobbleItem::IPodScrobbleItem( const QList<Track>& tracks )
    :ActivityListItem(), m_info( 0 ), m_tracks( tracks )
{
    setupUi();
    m_timestamp = QDateTime::currentDateTime();
    finishUi();
}


void IPodScrobbleItem::finishUi()
{
    // set the text for the row
    QString format = (m_tracks.count() == 1 ?
        tr("iPod '%2' (%1 track)"):
        tr("iPod '%2' (%1 tracks)"));

    setText( format.arg( QString::number(m_tracks.count()), m_tracks[0].extra("deviceId") ) );

    updateTimestamp();

    m_info = new IPodScrobbleInfoWidget( m_tracks );
    m_info->hide();
}


IPodScrobbleItem::IPodScrobbleItem( const QDomElement& element )
    :ActivityListItem(), m_info( 0 )
{
    setupUi();

    // Get the timestamp
    m_timestamp.setTime_t( element.attribute("timestamp").toUInt() );

    // read in the tracks
    for (QDomNode n = element.firstChild(); !n.isNull(); n = n.nextSibling())
        m_tracks.append( Track( n.toElement() ) );

    finishUi();
}


QDomElement
IPodScrobbleItem::toDomElement( QDomDocument xml ) const
{
    QDomElement element = xml.createElement( "track" );
    element.setAttribute("iPodScrobble", "1");
    element.setAttribute("timestamp", QString::number( m_timestamp.toTime_t() ) );

    foreach ( const Track& track, m_tracks )
        element.appendChild( track.toDomElement( xml ) );

    return element;
}


QWidget*
IPodScrobbleItem::infoWidget() const
{
    return m_info;
}



