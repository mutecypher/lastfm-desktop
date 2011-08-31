#include <QApplication>
#include <QHeaderView>

#include "lib/unicorn/dialogs/ShareDialog.h"
#include "lib/unicorn/dialogs/TagDialog.h"

#include "../Services/ScrobbleService.h"
#include "../ActivityListModel.h"

#include "ActivityListWidget.h"
#include "CheckableDelegate.h"
#include "TrackDelegate.h"
#include "MetadataWidget.h"

ActivityListWidget::ActivityListWidget( QWidget* parent )
    :QTreeView( parent )
{
    setModel( m_model = new ActivityListModel );
    QStyledItemDelegate* delegate = new CheckableDelegate(this);
    setItemDelegate( delegate );
    setItemDelegateForColumn( 0, new TrackDelegate(this));
    header()->setStretchLastSection( false );
    setAttribute( Qt::WA_MacShowFocusRect, false );
    viewport()->setAttribute( Qt::WA_Hover, true );
    viewport()->setMouseTracking( true );
    setSelectionMode( NoSelection );
    header()->setResizeMode( 0, QHeaderView::Stretch );

    for ( int i = 1; i < m_model->columnCount(); i++ )
        header()->setResizeMode( i, QHeaderView::ResizeToContents );

    header()->setMinimumSectionSize( 0 );
    setHeaderHidden( true );
    setRootIsDecorated( false );
    setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );

    connect( this, SIGNAL( clicked(QModelIndex) ), SLOT(onItemClicked(QModelIndex)));

    connect( m_model, SIGNAL(refreshing(bool)), SIGNAL(refreshing(bool)) );
}



void 
ActivityListWidget::onItemClicked( const QModelIndex& index ) 
{
    if( index.column() == 2 && index.data( ActivityListModel::HoverStateRole ).toBool() )
    {
        TagDialog* td = new TagDialog( index.data(ActivityListModel::TrackRole).value<Track>(), window() ); 
        td->raise(); 
        td->show(); 
        td->activateWindow();
    }
    else if( index.column() == 3 && index.data( ActivityListModel::HoverStateRole ).toBool() )
    {
        ShareDialog* td = new ShareDialog( index.data(ActivityListModel::TrackRole).value<Track>(), window() ); 
        td->raise(); 
        td->show(); 
        td->activateWindow(); 
        return;
    }
    else
    {
        emit trackClicked( index.data(ActivityListModel::TrackRole).value<Track>());
    }
}


void
ActivityListWidget::refresh()
{
    m_model->refresh();
}

