#ifndef SCROBBLESWIDGET_H
#define SCROBBLESWIDGET_H

#include <QWidget>

namespace lastfm { class Track; }
using lastfm::Track;

class ScrobblesWidget : public QWidget
{
    Q_OBJECT
private:
    struct
    {
        class QPushButton* refresh;
        class ActivityListWidget* activityList;
    } ui;

public:
    explicit ScrobblesWidget(QWidget *parent = 0);

    void refresh();

signals:
    void trackClicked( const Track& );

private slots:
    void onRefreshing( bool refreshing );
};

#endif // SCROBBLESWIDGET_H
