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
    setItemDelegate( delegate);
    setItemDelegateForColumn( 0, new TrackDelegate(this));
    header()->setStretchLastSection( false );
    setAttribute( Qt::WA_MacShowFocusRect, false );
    viewport()->setAttribute( Qt::WA_Hover, true );
    viewport()->setMouseTracking( true );
    setSelectionBehavior( SelectItems );
    header()->setResizeMode( 0, QHeaderView::Stretch );

    for( int i = 1; i < m_model->columnCount(); i++ )
    {
        header()->setResizeMode( i, QHeaderView::ResizeToContents );
    }
    header()->setMinimumSectionSize( 0 );
    setHeaderHidden( true );
    setRootIsDecorated( false );

    connect( &ScrobbleService::instance(), SIGNAL( trackStarted(Track, Track) ), m_model, SLOT( onTrackStarted(Track) ) );
    connect( qApp, SIGNAL( sessionChanged( unicorn::Session* )), m_model, SLOT(onSessionChanged( unicorn::Session* )));
    connect( this, SIGNAL(clicked(QModelIndex)),SLOT(onItemClicked(QModelIndex)));
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
        return;
    }
    if( index.column() == 3 && index.data( ActivityListModel::HoverStateRole ).toBool() )
    {
        ShareDialog* td = new ShareDialog( index.data(ActivityListModel::TrackRole).value<Track>(), window() ); 
        td->raise(); 
        td->show(); 
        td->activateWindow(); 
        return;
    }
    emit trackClicked( index.data(ActivityListModel::TrackRole).value<Track>());
}



