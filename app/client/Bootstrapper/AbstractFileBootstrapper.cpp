/**************************************************************************
*   Copyright (C) 2005 - 2007 by                                          *
*      Jono Cole, Last.fm Ltd <jono@last.fm>                              *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
***************************************************************************/

#include "AbstractFileBootstrapper.h"
#include <lastfm/misc.h>
#include <QFile>
#include <QTextStream>

static const int k_maxPlaysPerTrack = 10000;
static const int k_maxTotalPlays = 300000;

static const QString XML_VERSION = "1.0";


AbstractFileBootstrapper::AbstractFileBootstrapper( QString product, QObject* parent )
                         : AbstractBootstrapper( parent ),
                           m_runningPlayCount( 0 )
{
    m_bootstrapElement = m_xmlDoc.createElement( "bootstrap" );
    m_xmlDoc.appendChild( m_bootstrapElement );
    m_bootstrapElement.setAttribute( "product", product );
    m_bootstrapElement.setAttribute( "version", XML_VERSION );

    m_savePath = lastfm::dir::runtimeData().path() + "/" +  product + "_bootstrap.xml";
}


AbstractFileBootstrapper::~AbstractFileBootstrapper(void)
{
}


static QDomElement
trackToDom( const Track& t, QDomDocument& d )
{
    QDomElement item = d.createElement( "item" );
    QDomElement artist = d.createElement( "artist" );
    QDomElement album = d.createElement( "album" );
    QDomElement track = d.createElement( "track" );
    QDomElement duration = d.createElement( "duration" );
    QDomElement timestamp = d.createElement( "timestamp" );
    QDomElement playcount = d.createElement( "playcount" );
    QDomElement filename = d.createElement( "filename" );
    QDomElement uniqueid = d.createElement( "uniqueID" );

    artist.appendChild( d.createTextNode( t.artist()));
    album.appendChild( d.createTextNode( t.album()));
    track.appendChild( d.createTextNode( t.title()));
    duration.appendChild( d.createTextNode( QString::number( t.duration())));
    timestamp.appendChild( d.createTextNode( QString::number( t.timestamp().toTime_t())));
    playcount.appendChild( d.createTextNode( t.extra( "playcount" )));
    filename.appendChild( d.createTextNode( t.url().toString()));
    uniqueid.appendChild( d.createTextNode( t.extra( "unique_id" )));
    
    item.appendChild( artist );
    item.appendChild( album );
    item.appendChild( track );
    item.appendChild( duration );
    item.appendChild( timestamp );
    item.appendChild( playcount );
    item.appendChild( filename );
    item.appendChild( uniqueid );
    return item;
}


bool
AbstractFileBootstrapper::appendTrack(Track& track)
{
    int playCount = track.extra( "playcount" ).toInt();
    m_runningPlayCount += playCount;
    if ( playCount > k_maxPlaysPerTrack ||
        m_runningPlayCount > k_maxTotalPlays )
    {
//        LOGL( 2, "Playcount for bootstrap exceeded maximum allowed. Track: " <<
//            track.playCount() << ", total: " << m_runningPlayCount );

        emit done( Bootstrap_Spam );
        return false;
    }
    const QDomElement& i = trackToDom( track,  m_xmlDoc );
    m_bootstrapElement.appendChild( i );
    return true;
}


void
AbstractFileBootstrapper::zipAndSend()
{
    QFile file( m_savePath );
    file.open( QIODevice::WriteOnly | QIODevice::Text );
    QTextStream stream( &file );

    stream.setCodec( "UTF-8" );
    stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    stream << m_xmlDoc.toString();

    file.close();

    QString zipPath = m_savePath + ".gz";
    zipFiles( m_savePath, zipPath );

    sendZip( zipPath );
}

