
#include <QVBoxLayout>
#include <QPushButton>

#include "ActivityListWidget.h"
#include "RefreshButton.h"

#include "ScrobblesWidget.h"

ScrobblesWidget::ScrobblesWidget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    layout->addWidget( ui.refresh = new RefreshButton( this ) );
    ui.refresh->setObjectName( "refresh" );
    ui.refresh->setAttribute( Qt::WA_LayoutUsesWidgetRect );

    onRefreshing( false );

    layout->addWidget( ui.activityList = new ActivityListWidget );
    ui.activityList->setAttribute( Qt::WA_LayoutUsesWidgetRect );

    connect( ui.activityList, SIGNAL(trackClicked(Track)), SIGNAL(trackClicked(Track)) );
    connect( ui.refresh, SIGNAL(clicked()), ui.activityList, SLOT(refresh()) );

    connect( ui.activityList, SIGNAL(refreshing(bool)), SLOT(onRefreshing(bool)));
}

void
ScrobblesWidget::refresh()
{
    ui.activityList->refresh();
}

void
ScrobblesWidget::onRefreshing( bool refreshing )
{
    ui.refresh->setText( refreshing ? tr( "Refreshing..." ) : tr( "Refresh Scrobbles" ) );
    ui.refresh->setEnabled( !refreshing );
}
