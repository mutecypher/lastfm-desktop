#ifndef RADIOWIDGET_H
#define RADIOWIDGET_H

#include <QPointer>
#include <QWidget>

#include "lib/unicorn/StylableWidget.h"

#include "../Services/RadioService/RadioService.h"

namespace unicorn { class Session; };

class RadioWidget : public StylableWidget
{
    Q_OBJECT
private:
    struct
    {
        class StylableWidget* personal;
        class StylableWidget* network;
        class StylableWidget* topArtists;
        class StylableWidget* recentStations;

        class PlayableItemWidget* library;
        class PlayableItemWidget* mix;
        class PlayableItemWidget* rec;
        class PlayableItemWidget* friends;
        class PlayableItemWidget* neighbours;
    } ui;

public:
    explicit RadioWidget(QWidget *parent = 0);

private slots:
    void onSessionChanged( unicorn::Session* session );

    void onTuningIn( const RadioStation& station );

    void onGotTopArtists();
    void onGotRecentStations();

private:
    QPointer<QWidget> m_main;

};

#endif // RADIOWIDGET_H
