#ifndef STATIONWIDGET_H
#define STATIONWIDGET_H

#include <QWidget>

#include <lastfm/RadioStation>

namespace Ui {
    class StationWidget;
}

class StationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StationWidget(QWidget *parent = 0);
    ~StationWidget();

    void addStation( const RadioStation& station, const QString& description );
    void recentStation( const RadioStation& station );
    void setTasteometerCompareScore( const QString& user, float score );

    void clear();

    class QTreeView* treeView() const;

private:
    void resizeEvent( QResizeEvent * event );
    void contextMenuEvent( QContextMenuEvent* event );

    RadioStation getStation();

private slots:
    void onDoubleClicked( const class QModelIndex& index );
    void onTuningIn( const RadioStation& station );
    void onTrackSpooled( const Track& track );

    void onPlay();
    void onPlayNext();
    void onTagFilter();

private:
    Ui::StationWidget *ui;

    QAction* m_playAction;
    QAction* m_playNextAction;

    class StationListModel* m_model;
};

#endif // STATIONWIDGET_H
