
#include <QWidget>
#include "ScrobblesWidget.h"
#include "MetadataWidget.h"
#include "lib/unicorn/layouts/SideBySideLayout.h"
#include <types/Track.h>
#include <QDebug>

#include "RecentTracksWidget.h"


RecentTracksWidget::RecentTracksWidget( QWidget* parent )
    :QWidget( parent )
{
    m_layout = new SideBySideLayout();
    setLayout( m_layout );
    m_layout->addWidget( m_scrobbles = new ScrobblesWidget );

    connect( m_scrobbles, SIGNAL( trackClicked(Track)), SLOT( onTrackClicked(Track)));
    connect( m_layout, SIGNAL( moveFinished(QLayoutItem*)), SLOT(onMoveFinished(QLayoutItem*)));
}


void
RecentTracksWidget::onTrackClicked( const Track& track )
{
    MetadataWidget* w;
    m_layout->addWidget( w = new MetadataWidget( track ));
    w->setBackButtonVisible( true );
    m_layout->moveForward();

    connect( w, SIGNAL(backClicked()), SLOT(onBackClicked()));
}

void
RecentTracksWidget::onBackClicked()
{
    m_layout->moveToWidget( m_scrobbles );
}

void
RecentTracksWidget::onMoveFinished( QLayoutItem* i )
{
    if( i->widget() == m_scrobbles )
    {
        QWidget* nextWidget = m_layout->nextWidget();

        if( nextWidget )
        {
            m_layout->removeWidget( nextWidget );
            nextWidget->deleteLater();
        }
    }
}
