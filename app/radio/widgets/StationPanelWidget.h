#ifndef STATIONPANELWIDGET_H
#define STATIONPANELWIDGET_H

#include <QWidget>

#include <lastfm/RadioStation>

namespace Ui {
    class StationPanelWidget;
}

class StationPanelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StationPanelWidget(QWidget *parent = 0);
    ~StationPanelWidget();

private slots:
    void onTuningIn( const RadioStation& station );

    void onRecentClicked();
    void onFriendsClicked();
    void onNeighboursClicked();
    void onArtistsClicked();
    void onTagsClicked();

    void onGotRecentStations();
    void onGotFriends();
    void onGotNeighbours();
    void onGotArtists();
    void onGotTags();

    void onFriendsSortClicked();

private:
    Ui::StationPanelWidget *ui;

    class QSortFilterProxyModel* m_friendsProxyModel;
};

#endif // STATIONPANELWIDGET_H
