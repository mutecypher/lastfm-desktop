#ifndef LFM_LIST_VIEW_WIDGET_H
#define LFM_LIST_VIEW_WIDGET_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QUrl>
#include <QAbstractItemView>
#include <QEvent>
#include "lib/DllExportMacro.h"

#include <QDebug>

class UNICORN_DLLEXPORT LfmDelegate :public QStyledItemDelegate {
Q_OBJECT
public:
    LfmDelegate( QAbstractItemView* parent ):QStyledItemDelegate(parent)
    {
        m_viewSize = parent->size();
        parent->installEventFilter( this );
    }

    virtual void paint( QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index ) const
    {
        p->eraseRect( opt.rect );
        if( index.data(Qt::DecorationRole).type() == QVariant::Icon ) {
            QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
            QRect iconRect = opt.rect.translated( 3, 3 );
            iconRect.setSize( QSize( 34, 34 ));
            icon.paint( p, iconRect );
            QSize iconSize = icon.actualSize( iconRect.size());
            
            if( iconSize.isEmpty()) iconSize = QSize( 34, 34 );

            iconRect.translate( ( iconRect.width() - iconSize.width()) / 2.0f,
                                ( iconRect.height() - iconSize.height()) /2.0f );
            iconRect.setSize( iconSize );
            p->drawRect( iconRect);
        }
        p->drawText( opt.rect.adjusted( 46, 3, -5, -5 ), index.data().toString());
        if( opt.state & QStyle::State_Selected )
            p->drawRect( opt.rect.adjusted( 0, 0, -1, -1 ) );
    }

    virtual QSize sizeHint( const QStyleOptionViewItem& opt, const QModelIndex& index ) const 
    {
        QFontMetrics fm( opt.font );
        int textWidth = fm.width( index.data().toString());
        return QSize( m_viewSize.width() / 2, 40 );
    }

    bool eventFilter( QObject* obj, QEvent* event )
    {
        if( event->type() == QEvent::Resize ) {
            QWidget* view = qobject_cast< QWidget* >(obj );
            
            if( !view ) return false;

            m_viewSize = view->size();
            emit sizeHintChanged( QModelIndex() );
        }
        return false;
    }

    QSize m_viewSize;
};

class LfmItem : public QObject {
    Q_OBJECT
public:
    LfmItem(QObject* p = 0):QObject(p){};
    LfmItem(const LfmItem& that):QObject(that.parent()){ *this = that; }

    LfmItem& operator=( const LfmItem& that ) {
        description = that.description;
        link = that.link;
        icon = that.icon;
        return *this;
    }

    QString description;
    QUrl link;
    QIcon icon;

    bool operator==( const LfmItem& that ) const { return this->description == that.description; }

    void loadImage( const QUrl& url );

signals:
    void updated();

protected slots:
    void onImageLoaded();

};

namespace lastfm {
class User;
class Artist;
}
using lastfm::User;
using lastfm::Artist;

class UNICORN_DLLEXPORT LfmListModel : public QAbstractListModel {
    Q_OBJECT
public:
     LfmListModel( QObject* parent=0 ):QAbstractListModel( parent ){}
     
     void addUser( const User& );
     void addArtist( const Artist& );

     int rowCount ( const QModelIndex & parent = QModelIndex() ) const
     {
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


#endif //LFM_LIST_VIEW_WIDGET_H

