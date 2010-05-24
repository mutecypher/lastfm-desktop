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
    icon = QIcon( px );
    emit updated();
}


void LfmItem::loadImage( const QUrl& url )
{
    QString imageUrl = url.toString();
   
    QNetworkReply* reply = lastfm::nam()->get(QNetworkRequest( url ));
    connect( reply, SIGNAL( finished()), this, SLOT( onImageLoaded()));
}


void
LfmListModel::addUser( const User& user )
{
    LfmItem* item = new LfmItem();
    item->description = user.name();
    item->link = user.www();
    item->loadImage( user.imageUrl(lastfm::Small, true ));

    beginInsertRows( QModelIndex(), rowCount(), rowCount());
    m_items << item;
    connect( item, SIGNAL(updated()), SLOT( itemUpdated()));
    endInsertRows();
}


void 
LfmListModel::addArtist( const Artist& artist )
{
    LfmItem* item = new LfmItem();
    item->description = artist.name();
    item->link = artist.www();
    item->loadImage( artist.imageUrl( lastfm::Small, true ));

    beginInsertRows( QModelIndex(), rowCount(), rowCount());
    m_items << item;
    connect( item, SIGNAL(updated()), SLOT( itemUpdated()));
    endInsertRows();   
}

void
LfmListModel::addCachedTrack( const Track& track )
{
    LfmItem* item = new LfmItem();
    item->description = track.toString();
    item->link = track.www();
    item->loadImage( track.imageUrl( lastfm::Small, true ));

    beginInsertRows( QModelIndex(), rowCount(), rowCount());
    m_items.insert( 0, item );
    connect( item, SIGNAL(updated()), SLOT( itemUpdated()));
    if ( m_items.count() > 5 )
        m_items.removeAt( 5 );
    endInsertRows();
}

void
LfmListModel::addScrobbledTrack( const Track& track )
{
    // do nothing as the track will already
    // be in the list from when it was cached
}


void
LfmListModel::itemUpdated()
{
    LfmItem* item = static_cast<LfmItem*>(sender());
    int index = m_items.indexOf( item );
    emit dataChanged( createIndex( index, 0), createIndex( index, 0));
}


QVariant 
LfmListModel::data( const QModelIndex & index, int role ) const
{
    const LfmItem& item = *(m_items[index.row()]);
    switch( role ) {
        case Qt::DisplayRole:
            return item.description;

        case Qt::DecorationRole:
            return item.icon;

        case Qt::UserRole:
            return item.link;

    }
    return QVariant();
}
