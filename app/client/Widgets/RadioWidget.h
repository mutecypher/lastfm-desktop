#ifndef RADIOWIDGET_H
#define RADIOWIDGET_H

#include <QPointer>
#include <QFrame>

namespace lastfm { class User; }
namespace lastfm { class RadioStation; }
namespace lastfm { class Track; }
using lastfm::RadioStation;
using lastfm::Track;

namespace unicorn { class Session; }

class RadioWidget : public QFrame
{
    Q_OBJECT
private:
    struct
    {
        class QFrame* nowPlayingFrame;
        class QFrame* nowPlayingSection;
        class QLabel* nowPlaying;

        class QFrame* personal;
        class QFrame* network;
        class QFrame* recentStations;

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
