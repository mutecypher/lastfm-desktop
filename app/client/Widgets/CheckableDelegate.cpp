
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>

#include "../Services/ScrobbleService/ScrobbleService.h"
#include "../ActivityListModel.h"

#include "CheckableDelegate.h"

CheckableDelegate::CheckableDelegate( QObject* parent )
    : QStyledItemDelegate( parent )
{
}

void
CheckableDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    Track track = index.data( ActivityListModel::TrackRole ).value<Track>();
    QDateTime timestamp = track.timestamp();
    bool isNowPlaying = timestamp.toTime_t() == 0 || track.sameObject( ScrobbleService::instance().currentTrack() );

    //const QImage& image = index.data( Qt::DecorationRole ).value<QImage>();
    QLinearGradient g( option.rect.topLeft(), option.rect.bottomLeft());
    g.setColorAt( 0, isNowPlaying ? QColor( 0xFFFCCA ) : QColor( 0xeeeeee ) );
    g.setColorAt( 1, isNowPlaying ? QColor( 0xeeebb9 ) : QColor( 0xdddddd ) );
    painter->fillRect( option.rect, g );

    painter->setPen(QColor(0xaaaaaa));
    painter->drawLine( option.rect.bottomLeft(), option.rect.bottomRight());
    const QIcon& icon = index.data(Qt::DecorationRole).value<QIcon>();

    bool hover = false;
    if( option.state & QStyle::State_MouseOver )
    {
        hover = testHover( index, option );
    }

    if( index.data( Qt::CheckStateRole ) == Qt::Checked )
    {
        painter->drawPixmap( iconRect( index, option ),
                             icon.pixmap(QSize( 21, 18 ),
                             hover ? QIcon::Selected : QIcon::Normal,
                             QIcon::On ));
    }
    else
    {
        painter->drawPixmap( iconRect( index, option ),
                             icon.pixmap(QSize( 21, 18 ),
                             hover ? QIcon::Selected : QIcon::Normal,
                             QIcon::Off ));
    }
}

bool
CheckableDelegate::testHover( const QModelIndex& index, const QStyleOptionViewItem& option ) const
{
    const QWidget* const parentWidget = qobject_cast<QWidget*>(parent());
    if( !parentWidget )
    {
        return true;
    }
    else
    {
        QPoint localCursorPos = parentWidget->mapFromGlobal( QCursor::pos());

        if( iconRect( index, option ).contains( localCursorPos ))
        {
            return true;
        }
    }
    return false;
}

bool
CheckableDelegate::editorEvent( QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index )
{
    if( event->type() == QEvent::MouseMove )
    {
        if( testHover( index, option ))
            model->setData( index, true, ActivityListModel::HoverStateRole );
    }

    if( event->type() != QEvent::MouseButtonRelease )
        return false;

    const QRect rect = iconRect( index, option );

    if( !rect.contains( ((QMouseEvent*)event)->pos()))
        return false;

    if( index.data( Qt::CheckStateRole ) == Qt::Checked )
        model->setData( index, Qt::Unchecked );
    else
        model->setData( index, Qt::Checked );

    return true;
}

QSize
CheckableDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    return iconRect( index, option ).size();
}

QRect
CheckableDelegate::iconRect( const QModelIndex& index, const QStyleOptionViewItem& option ) const
{
    const QIcon& icon = index.data(Qt::DecorationRole).value<QIcon>();
    const QSize iconSize = icon.actualSize( QSize( 21, 18 ));
    return QRect( option.rect.topLeft() + QPoint( option.rect.width() /2.0f, 5) - QPoint( iconSize.width() / 2, 0 ),
                  iconSize);
}

QString
CheckableDelegate::prettyTime( const QDateTime& timestamp ) const
{
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
