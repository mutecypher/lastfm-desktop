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
    void refreshing( bool refreshing );

public slots:
    void refresh();

private slots: 
    void onItemClicked( const QModelIndex& index );

    void onShareLastFm();
    void onShareTwitter();
    void onShareFacebook();

private:
    class ActivityListModel* m_model;

    QModelIndex m_shareIndex;
};


#endif //ACTIVITY_LIST_WIDGET_H

