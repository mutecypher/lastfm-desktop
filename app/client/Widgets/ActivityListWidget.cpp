#include "ActivityListWidget.h"
#include "../Services/ScrobbleService.h"
#include <QApplication>
#include <QHeaderView>
#include "../ActivityListModel.h"

class CheckableDelegate : public QStyledItemDelegate {
public:
    CheckableDelegate( QObject* parent = 0 ) : QStyledItemDelegate( parent ){}
    virtual void paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const {
        QLinearGradient g( option.rect.topLeft(), option.rect.bottomLeft());
        g.setColorAt( 0, QColor( 0xeeeeee ) );
        g.setColorAt( 1, QColor( 0xdddddd ) );
        painter->fillRect( option.rect, g );
        painter->setPen(QColor(0xaaaaaa));
        painter->drawLine( option.rect.bottomLeft(), option.rect.bottomRight());
        const QIcon& icon = index.data(Qt::DecorationRole).value<QIcon>();
      
        bool hover = false;
        if( option.state & QStyle::State_MouseOver ) {
            hover = testHover( index, option );
        }
         
        if( index.data( Qt::CheckStateRole ) == Qt::Checked ) {
            painter->drawPixmap( iconRect( index, option ), 
                                 icon.pixmap(QSize( 84, 84 ), 
                                 hover ? QIcon::Selected : QIcon::Normal, 
                                 QIcon::On ));
        } else {
            painter->drawPixmap( iconRect( index, option ), 
                                 icon.pixmap(QSize( 84, 84 ), 
                                 hover ? QIcon::Selected : QIcon::Normal, 
                                 QIcon::Off ));
        }
    }

    bool testHover( const QModelIndex& index, const QStyleOptionViewItem& option ) const {
        const QWidget* const parentWidget = qobject_cast<QWidget*>(parent());
        if( !parentWidget ) {
            return true;
        } else {
            QPoint localCursorPos = parentWidget->mapFromGlobal( QCursor::pos());
            if( iconRect( index, option ).contains( localCursorPos )) {
                return true;
            }
        }
        return false;
    }

    virtual bool editorEvent( QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index ) {
        if( event->type() == QEvent::MouseMove ) { if( testHover( index, option )) model->setData( index, true, ActivityListModel::HoverStateRole ); }
        if( event->type() != QEvent::MouseButtonRelease ) return false;

        const QRect rect = iconRect( index, option );
        if( !rect.contains( ((QMouseEvent*)event)->pos())) return false;

        if( index.data( Qt::CheckStateRole ) == Qt::Checked )
            model->setData( index, Qt::Unchecked );
        else
            model->setData( index, Qt::Checked );

        return true;
    }
    
    virtual QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const {
        return iconRect( index, option ).size();
    }

private:
    QRect iconRect( const QModelIndex& index, const QStyleOptionViewItem& option ) const {
        const QIcon& icon = index.data(Qt::DecorationRole).value<QIcon>();
        const QSize iconSize = icon.actualSize( QSize( 84, 84 ));
        return QRect( option.rect.topLeft() + QPoint( option.rect.width() /2.0f, 5) - QPoint( iconSize.width() / 2, 0 ),
                      iconSize);
    }

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

ActivityListWidget::ActivityListWidget( QWidget* parent )
                   :QTreeView( parent )
{
    
    setModel( m_model = new ActivityListModel );
    QStyledItemDelegate* delegate = new CheckableDelegate(this);
    setItemDelegate( delegate);
    setItemDelegateForColumn( 0, new TrackDelegate(this));
    header()->setStretchLastSection( false );
    setAttribute( Qt::WA_MacShowFocusRect, false );
    viewport()->setAttribute( Qt::WA_Hover, true );
    viewport()->setMouseTracking( true );
    setSelectionBehavior( SelectItems );
    header()->setResizeMode( 0, QHeaderView::Stretch );
    for( int i = 1; i < m_model->columnCount(); i++ ) {
        header()->setResizeMode( i, QHeaderView::ResizeToContents );
    }
    header()->setMinimumSectionSize( 0 );
    setHeaderHidden( true );
    setRootIsDecorated( false );

    connect( &ScrobbleService::instance(), SIGNAL( trackStarted(Track, Track) ), m_model, SLOT( onTrackStarted(Track) ) );
    connect( qApp, SIGNAL( sessionChanged( unicorn::Session* )), m_model, SLOT(onSessionChanged( unicorn::Session* )));
    connect( this, SIGNAL(clicked(QModelIndex)),SLOT(onItemClicked(QModelIndex)));
}

#include "../../../lib/unicorn/dialogs/ShareDialog.h"
#include "../../../lib/unicorn/dialogs/TagDialog.h"
#include "MetadataWidget.h"

void 
ActivityListWidget::onItemClicked( const QModelIndex& index ) 
{
    if( index.column() == 2 && index.data( ActivityListModel::HoverStateRole ).toBool() ) {
        TagDialog* td = new TagDialog( index.data(ActivityListModel::TrackRole).value<Track>(), window() ); 
        td->raise(); 
        td->show(); 
        td->activateWindow();
        return;
    }
    if( index.column() == 3 && index.data( ActivityListModel::HoverStateRole ).toBool() ) {
        ShareDialog* td = new ShareDialog( index.data(ActivityListModel::TrackRole).value<Track>(), window() ); 
        td->raise(); 
        td->show(); 
        td->activateWindow(); 
        return;
    }
    emit trackClicked( index.data(ActivityListModel::TrackRole).value<Track>());
}



