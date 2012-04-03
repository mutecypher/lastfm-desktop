#ifndef RADIOWIDGET_H
#define RADIOWIDGET_H

#include <QPointer>
#include <QWidget>

#include "lib/unicorn/StylableWidget.h"

namespace lastfm { class User; }
namespace lastfm { class RadioStation; }
namespace lastfm { class Track; }
using lastfm::RadioStation;
using lastfm::Track;

namespace unicorn { class Session; }

class RadioWidget : public StylableWidget
{
    Q_OBJECT
private:
    struct
    {
        class QLabel* nowPlaying;

        class StylableWidget* personal;
        class StylableWidget* network;
        class StylableWidget* recentStations;

        class PlayableItemWidget* lastStation;

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
    void onGotUserInfo( const lastfm::User& userDetails );

    void onTuningIn( const RadioStation& station );
    void onRadioStopped();
    void onTrackStarted( const Track& track , const Track& oldTrack );

    void onGotRecentStations();

private:
    void changeUser( const QString& newUsername );

private:
    QPointer<QWidget> m_main;

    QString m_currentUsername;
};

#endif // RADIOWIDGET_H
