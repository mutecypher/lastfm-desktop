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

#include <QCoreApplication>
#include <QDomDocument>
#include <QFile>

#include "LfmListViewWidget.h"
#include <lastfm/User>
#include <lastfm/Artist>
#include <lastfm/Track>

#include <iostream>

void
LfmItem::onImageLoaded()
{
    QPixmap px;
    px.loadFromData(static_cast<QNetworkReply*>(sender())->readAll());
    m_icon = QIcon( px );
    emit updated();
}


void LfmItem::loadImage( const QUrl& url )
{
    QString imageUrl = url.toString();
   
    QNetworkReply* reply = lastfm::nam()->get(QNetworkRequest( url ));
    connect( reply, SIGNAL( finished()), this, SLOT( onImageLoaded()));
}


void
LfmListModel::addUser( const User& a_user )
{
    User* user = new User;
    *user = a_user;
    LfmItem* item = new LfmItem( user );
    item->loadImage( user->imageUrl(lastfm::Small, true ));

    beginInsertRows( QModelIndex(), rowCount(), rowCount());
    m_items << item;
    connect( item, SIGNAL(updated()), SLOT( itemUpdated()));
    endInsertRows();
}


void 
LfmListModel::addArtist( const Artist& a_artist )
{
    Artist* artist = new Artist;
    *artist = a_artist;
    LfmItem* item = new LfmItem( artist );
    item->loadImage( artist->imageUrl( lastfm::Small, true ));

    beginInsertRows( QModelIndex(), rowCount(), rowCount());
    m_items << item;
    connect( item, SIGNAL(updated()), SLOT( itemUpdated()));
    endInsertRows();   
}

void
LfmListModel::addCachedTrack( const Track& a_track )
{
    MutableTrack( a_track ).setExtra( "scrobbleStatus", "cached" );

    Track* track = new Track;
    *track = a_track;
    LfmItem* item = new LfmItem( track );
    item->loadImage( track->imageUrl( lastfm::Small, true ));

    beginInsertRows( QModelIndex(), rowCount(), rowCount());
    m_items.insert( 0, item );
    connect( item, SIGNAL(updated()), SLOT( itemUpdated()));
    if ( m_items.count() > 5 )
        m_items.removeAt( 5 );
    endInsertRows();
}

void
LfmListModel::addScrobbledTrack( const Track& a_track )
{
    MutableTrack( a_track ).setExtra( "scrobbleStatus", "scrobbled" );
}


void
LfmListModel::itemUpdated()
{
    LfmItem* item = static_cast<LfmItem*>(sender());
    int index = m_items.indexOf( item );
    emit dataChanged( createIndex( index, 0), createIndex( index, 0));
    emit layoutChanged();
}


QVariant 
LfmListModel::data( const QModelIndex & index, int role ) const
{
    if( index.row() >= m_items.count()) return QVariant();

    const LfmItem& item = *(m_items[index.row()]);
    switch( role ) {
        case Qt::DisplayRole:
            return item.m_type->toString();

        case Qt::DecorationRole:
            return item.m_icon;

        case CursorRole:
            return Qt::PointingHandCursor;

        case WwwRole:
            return item.m_type->www();

    }
    return QVariant();
}

void
LfmListModel::read( QString path )
{
    m_items.clear();

    QFile file( path );
    file.open( QFile::Text | QFile::ReadOnly );
    QTextStream stream( &file );
    stream.setCodec( "UTF-8" );

    QDomDocument xml;
    xml.setContent( stream.readAll() );

    for (QDomNode n = xml.documentElement().firstChild(); !n.isNull(); n = n.nextSibling())
    {
        // TODO: recognise all the other AbstractType classes
        if (n.nodeName() == "track")
        {
            emit layoutAboutToBeChanged();
            Track* track = new Track( n.toElement() );
            LfmItem* item = new LfmItem( track );
            item->loadImage( track->imageUrl( lastfm::Small, true ));
            m_items << item;
            emit layoutChanged();
        }
    }
}


void
LfmListModel::write( QString path ) const
{
    if (m_items.isEmpty())
    {
        QFile::remove( path );
    }
    else
    {
        QDomDocument xml;
        QDomElement e = xml.createElement( "recent_tracks" );
        e.setAttribute( "product", QCoreApplication::applicationName() );
        e.setAttribute( "version", "2" );

        foreach (LfmItem* item, m_items)
            e.appendChild( item->m_type->toDomElement( xml ) );

        xml.appendChild( e );

        QFile file( path );
        file.open( QIODevice::WriteOnly | QIODevice::Text );

        QTextStream stream( &file );
        stream.setCodec( "UTF-8" );
        stream << "<?xml version='1.0' encoding='utf-8'?>\n";
        stream << xml.toString( 2 );
    }
}

LfmListView::LfmListView(QWidget *parent):
   QListView(parent),
   m_lastRow(-1)
{
   setMouseTracking(true);
}

#include <QMouseEvent>
void LfmListView::mouseMoveEvent(QMouseEvent *event)
{
   QAbstractItemModel *m(model());

   if (m)
   {
      QModelIndex index = indexAt(event->pos());
      if (index.isValid())
      {
         // When the index is valid, compare it to the last row.
         // Only do something when the the mouse has moved to a new row.
         if (index.row() != m_lastRow)
         {
            m_lastRow = index.row();
            // Request the data for the CursorRole.
            QVariant data = m->data(index, LfmListModel::CursorRole );

            Qt::CursorShape shape = Qt::ArrowCursor;
            if (!data.isNull())
               shape = static_cast<Qt::CursorShape>(data.toInt());
            setCursor(shape);
         }
      }
      else
      {
         if (m_lastRow != -1)
            setCursor(Qt::ArrowCursor);
         m_lastRow = -1;
      }
   }
   QListView::mouseMoveEvent(event);
}
