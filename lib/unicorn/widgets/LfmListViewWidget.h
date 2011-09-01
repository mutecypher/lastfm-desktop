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

#ifndef LFM_LIST_VIEW_WIDGET_H
#define LFM_LIST_VIEW_WIDGET_H


#include <types/AbstractType.h>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QUrl>
#include <QAbstractItemView>
#include <QEvent>
#include "lib/DllExportMacro.h"
#include <QListView>
#include <QFontMetrics>

#include <QDebug>

namespace lastfm { class AbstractType; };
using lastfm::AbstractType;

class UNICORN_DLLEXPORT LfmDelegate :public QStyledItemDelegate {
    Q_OBJECT
    Q_PROPERTY(QPixmap defaultImage READ defaultImage WRITE setDefaultImage);

public:
    LfmDelegate( QAbstractItemView* parent );

    virtual void paint( QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index ) const;
    virtual QSize sizeHint( const QStyleOptionViewItem& opt, const QModelIndex& index ) const;
    bool eventFilter( QObject* obj, QEvent* event );

    QPixmap defaultImage() const;
    void setDefaultImage( QPixmap defaultImage );

private:
    QSize m_viewSize;
    QPixmap m_defaultImage;
};

class LfmItem : public QObject {
    Q_OBJECT
public:
    LfmItem(AbstractType* type, QObject* parent = 0)
        :QObject( parent ), m_type( type )
    {;}

    ~LfmItem() { delete m_type; }

    bool operator==( const LfmItem& that ) const { return this->m_type->toString() == that.m_type->toString(); }

    void loadImage( const QUrl& url );

    QIcon m_icon;
    AbstractType* m_type;

signals:
    void updated();

protected slots:
    void onImageLoaded();
};

namespace lastfm {
class User;
class Artist;
class Track;
}
using lastfm::User;
using lastfm::Artist;
using lastfm::Track;

class UNICORN_DLLEXPORT LfmListModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum DataRole { WwwRole = Qt::UserRole,
                    CursorRole};

    LfmListModel( QObject* parent=0 ):QAbstractListModel( parent ){}

    void addUser( const User& );
    void addArtist( const Artist& );
    void addCachedTrack( const Track& );
    void addScrobbledTrack( const Track& );

    void read(QString path);
    void write(QString path) const;

    int rowCount ( const QModelIndex& parent = QModelIndex() ) const
    {
        Q_UNUSED(parent);
        return m_items.length();
    }

    QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;

    void clear()
    {
    if( m_items.isEmpty()) return;
    beginRemoveRows( QModelIndex(), 0, rowCount() -1 );
        foreach( LfmItem* item, m_items ) {
            item->deleteLater();
            m_items.removeAll( item );
        }
    endRemoveRows();
    }
     
protected slots:
    void itemUpdated();

protected:
    QList<LfmItem*> m_items;

};


#include <QListView>
class UNICORN_DLLEXPORT LfmListView : public QListView {
   Q_OBJECT
public:
   LfmListView(QWidget *parent = 0);

protected:
   virtual void mouseMoveEvent(QMouseEvent *event);

private:
   int m_lastRow;
};


#endif //LFM_LIST_VIEW_WIDGET_H

