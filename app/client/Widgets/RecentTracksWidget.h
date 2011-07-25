#ifndef RECENT_TRACKS_WIDGET_H_
#define RECENT_TRACKS_WIDGET_H_

#include <QWidget>
#include "ActivityListWidget.h"
#include "MetadataWidget.h"
#include "lib/unicorn/layouts/SideBySideLayout.h"
#include <lastfm/Track>
#include <QDebug>

class RecentTracksWidget : public QWidget 
{
    Q_OBJECT
    public:
        RecentTracksWidget( QWidget* parent = 0 )
        :QWidget( parent ) {
            layout = new SideBySideLayout();
            setLayout( layout );
            layout->addWidget( activityList = new ActivityListWidget );
            connect( activityList, SIGNAL( trackClicked(Track)), SLOT( onTrackClicked(Track)));
            connect( layout, SIGNAL( moveFinished(QLayoutItem*)), SLOT(onMoveFinished(QLayoutItem*)));
        }

    protected slots:
        void onTrackClicked( const Track& track ) {
            MetadataWidget* w;
            layout->addWidget( w = new MetadataWidget( track ));
            layout->moveForward();
            connect( w, SIGNAL(backClicked()), SLOT(onBackClicked()));
        }

        void onBackClicked() {
            layout->moveToWidget( activityList );
        }

        void onMoveFinished( QLayoutItem* i ) {
            if( i->widget() == activityList ) {
                QWidget* nextWidget = layout->nextWidget();
                if( nextWidget ) {
                    layout->removeWidget( nextWidget );
                    nextWidget->deleteLater();
                }
            }
        }

    protected:
        SideBySideLayout* layout;
        ActivityListWidget* activityList;
};

#endif //RECENT_TRACKS_WIDGET_H_
