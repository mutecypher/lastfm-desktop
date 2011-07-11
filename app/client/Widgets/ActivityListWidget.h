#ifndef ACTIVITY_LIST_WIDGET_H
#define ACTIVITY_LIST_WIDGET_H

#include <QTreeView>
#include <QMouseEvent>
class ActivityListWidget : public QTreeView {
    Q_OBJECT
public:
    ActivityListWidget( QWidget* parent );

protected:
    int sizeHintForColumn( int column ) const {
        if( 1 == column )
            return 40;
        
        return QTreeView::sizeHintForColumn( column );
    }

private slots:
/*    void refreshRecentTracks( User user = User() );
    void onGotRecentTracks();   */
    void onItemClicked( const QModelIndex& index );

private:
    class ActivityListModel* m_model;
};

#include <QPainter>
#include <QStyledItemDelegate>
#include "../ActivityListModel.h"
class TrackDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    TrackDelegate( QObject* parent = 0 ) : QStyledItemDelegate( parent ){}
    virtual void paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const {

        const int timestampWidth=100;
        const QImage& image = index.data( Qt::DecorationRole ).value<QImage>();
        QLinearGradient g( option.rect.topLeft(), option.rect.bottomLeft());
        g.setColorAt( 0, QColor( 0xeeeeee ) );
        g.setColorAt( 1, QColor( 0xdddddd ) );
        painter->fillRect( option.rect, g );
        
        painter->setPen(QColor(0xaaaaaa));
        painter->drawLine( option.rect.bottomLeft(), option.rect.bottomRight());
        
        painter->drawImage( option.rect.topLeft() + QPoint( 20, 10 ), image );

        QFont f("Lucida Grande");
        f.setBold( true );
        f.setPointSize( 14 );
        painter->setPen(QColor(0x333333));
        painter->setFont( f );

        painter->setPen(Qt::white);
        painter->drawText( option.rect.left() + 94, option.rect.top() + 26, index.data( ActivityListModel::TrackNameRole ).toString());
        painter->setPen(QColor(0x333333));
        painter->drawText( option.rect.left() + 94, option.rect.top() + 25, index.data( ActivityListModel::TrackNameRole ).toString());
        f.setBold( false );
        painter->setFont( f );
        painter->setPen(Qt::white);
        painter->drawText( option.rect.left() + 94, option.rect.top() + 46, index.data( ActivityListModel::ArtistNameRole ).toString());
        painter->setPen(QColor(0x333333));
        painter->drawText( option.rect.left() + 94, option.rect.top() + 45, index.data( ActivityListModel::ArtistNameRole ).toString());

        painter->setPen(QColor(0x777777));
        
        f.setPointSize( 11 );
        painter->setFont( f );
        painter->drawText( option.rect.right() - timestampWidth, option.rect.top() + 25, prettyTime(index.data( ActivityListModel::TimeStampRole ).toDateTime()));
    }

private:
    QString prettyTime( const QDateTime& timestamp ) const {
        QString dateFormat( "d MMM h:mmap" );
        QDateTime now = QDateTime::currentDateTime();
        int secondsAgo = timestamp.secsTo( now );

        if ( secondsAgo < (60 * 60) )
        {
            // Less than an hour ago
            int minutesAgo = ( timestamp.secsTo( now ) / 60 );
            return (minutesAgo == 1 ? tr( "%1 minute ago" ) : tr( "%1 minutes ago" ) ).arg( QString::number( minutesAgo ) );
        }
        else if ( secondsAgo < (60 * 60 * 6) || now.date() == timestamp.date() )
        {
            // Less than 6 hours ago or on the same date
            int hoursAgo = ( timestamp.secsTo( now ) / (60 * 60) );
            return (hoursAgo == 1 ? tr( "%1 hour ago" ) : tr( "%1 hours ago" ) ).arg( QString::number( hoursAgo ) );
        }
        else
        {
            return timestamp.toString( dateFormat );
            // We don't need to set the timer because this date will never change
        }
    }
};


#endif //ACTIVITY_LIST_WIDGET_H

