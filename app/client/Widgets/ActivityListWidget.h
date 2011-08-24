#ifndef ACTIVITY_LIST_WIDGET_H
#define ACTIVITY_LIST_WIDGET_H

#include <QTreeView>
#include <QMouseEvent>
namespace lastfm{ class Track; }
using lastfm::Track;

class ActivityListWidget : public QTreeView {
    Q_OBJECT
public:
    ActivityListWidget( QWidget* parent = 0 );

signals:
    void trackClicked( const Track& );

protected:


private slots:
/*    void refreshRecentTracks( User user = User() );
    void onGotRecentTracks();   */
    void onItemClicked( const QModelIndex& index );

private:
    class ActivityListModel* m_model;
};


#endif //ACTIVITY_LIST_WIDGET_H

