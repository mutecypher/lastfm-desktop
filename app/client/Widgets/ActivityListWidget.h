#ifndef ACTIVITY_LIST_WIDGET_H
#define ACTIVITY_LIST_WIDGET_H

#include <QListWidget>
#include <QMouseEvent>
#include <QPoint>

namespace lastfm{ class Track; }
using lastfm::Track;

class ActivityListWidget : public QListWidget
{
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

    void onGotBuyLinks();
    void onBuyActionTriggered( QAction* buyAction );

private:
    QString price( const QString& price, const QString& currency ) const;

private:
    class ActivityListModel* m_model;

    QModelIndex m_shareIndex;

    QPoint m_buyCursor;
};


#endif //ACTIVITY_LIST_WIDGET_H

