
#include <QWidget>
#include "ActivityListWidget.h"
#include "MetadataWidget.h"
#include "lib/unicorn/layouts/SideBySideLayout.h"
#include <lastfm/Track>
#include <QDebug>

#include "RecentTracksWidget.h"


RecentTracksWidget::RecentTracksWidget( QWidget* parent )
    :QWidget( parent )
{
    layout = new SideBySideLayout();
    setLayout( layout );
    layout->addWidget( activityList = new ActivityListWidget );

    connect( activityList, SIGNAL( trackClicked(Track)), SLOT( onTrackClicked(Track)));
    connect( layout, SIGNAL( moveFinished(QLayoutItem*)), SLOT(onMoveFinished(QLayoutItem*)));
}


void
RecentTracksWidget::onTrackClicked( const Track& track )
{
    MetadataWidget* w;
    layout->addWidget( w = new MetadataWidget( track ));
    w->setBackButtonVisible( true );
    layout->moveForward();

    connect( w, SIGNAL(backClicked()), SLOT(onBackClicked()));
}

void
RecentTracksWidget::onBackClicked()
{
    layout->moveToWidget( activityList );
}

void
RecentTracksWidget::onMoveFinished( QLayoutItem* i )
{
    if( i->widget() == activityList )
    {
        QWidget* nextWidget = layout->nextWidget();

        if( nextWidget )
        {
            layout->removeWidget( nextWidget );
            nextWidget->deleteLater();
        }
    }
}
