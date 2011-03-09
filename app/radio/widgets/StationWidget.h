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

    void addStation( const RadioStation& station );

private:
    Ui::StationWidget *ui;
};

#endif // STATIONWIDGET_H
