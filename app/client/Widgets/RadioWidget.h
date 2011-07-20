#ifndef RADIOWIDGET_H
#define RADIOWIDGET_H

#include <QWidget>

namespace unicorn { class Session; };

class RadioWidget : public QWidget
{
    Q_OBJECT
private:
    struct
    {
        class StylableWidget* personal;
        class StylableWidget* network;
        class StylableWidget* topArtists;
        class StylableWidget* recentStations;
    } ui;

public:
    explicit RadioWidget(QWidget *parent = 0);

private slots:
    void onSessionChanged( unicorn::Session* session );

    void onGotTopArtists();
    void onGotRecentStations();

};

#endif // RADIOWIDGET_H
