#include "LfmListViewWidget.h"
#include <lastfm.h>
#include <lastfm/User>
#include <lastfm/Artist>

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
