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
        return QRect( option.rect.center() - QPoint( iconSize.width() / 2, iconSize.height() / 2 ),
                      iconSize);
    }
};




ActivityListWidget::ActivityListWidget( QWidget* parent )
                   :QTreeView( parent )
{
    
    setModel( m_model = new ActivityListModel );
    QStyledItemDelegate* delegate = new CheckableDelegate(this);
    setItemDelegateForColumn( 0, new TrackDelegate());
    setItemDelegateForColumn( 1, delegate);
    setItemDelegateForColumn( 2, delegate);
    header()->setStretchLastSection( false );
    setAttribute( Qt::WA_MacShowFocusRect, false );
    viewport()->setAttribute( Qt::WA_Hover, true );
    viewport()->setMouseTracking( true );
    setSelectionBehavior( SelectItems );
    header()->setResizeMode( 0, QHeaderView::Stretch );
    header()->setResizeMode( 1, QHeaderView::ResizeToContents );
    header()->setResizeMode( 2, QHeaderView::ResizeToContents );
    header()->setMinimumSectionSize( 0 );
    setHeaderHidden( true );
    setRootIsDecorated( false );
    connect( &ScrobbleService::instance(), SIGNAL( trackStarted(Track, Track) ), m_model, SLOT( onTrackStarted(Track) ) );
    connect( qApp, SIGNAL( sessionChanged( unicorn::Session* )), m_model, SLOT(onSessionChanged( unicorn::Session* )));
    connect( this, SIGNAL(clicked(QModelIndex)),SLOT(onItemClicked(QModelIndex)));
}

#include "../../../lib/unicorn/dialogs/TagDialog.h"
void 
ActivityListWidget::onItemClicked( const QModelIndex& index ) 
{
    if( index.column() == 2 && index.data( ActivityListModel::HoverStateRole ).toBool() ) {
        TagDialog* td = new TagDialog( index.data(ActivityListModel::TrackRole).value<Track>(), window() ); 
        td->raise(); 
        td->show(); 
        td->activateWindow(); 
    }
}



