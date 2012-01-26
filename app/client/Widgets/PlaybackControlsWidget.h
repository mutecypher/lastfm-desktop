#ifndef PLAYBACKCONTROLS_H
#define PLAYBACKCONTROLS_H

#include <QAction>
#include <QWidget>
#include <QPointer>

#include <lastfm/RadioStation.h>

#include "lib/unicorn/StylableWidget.h"

namespace Ui { class PlaybackControlsWidget; }

class QMovie;

class PlaybackControlsWidget : public StylableWidget
{
    Q_OBJECT

public:
    explicit PlaybackControlsWidget(QWidget* parent = 0);
    ~PlaybackControlsWidget();

public:
    Q_PROPERTY( bool scrobbleTrack READ scrobbleTrack WRITE setScrobbleTrack );

    bool scrobbleTrack() { return m_scrobbleTrack; }
    void setScrobbleTrack( bool scrobbleTrack );

    void addToMenu( class QMenu& menu );

private slots:
    void onActionsChanged();
    void onSpace();
    void onPlayClicked( bool checked );
    void onPlayTriggered( bool checked );
    void onSkipClicked();
    void onLoveClicked( bool loved );
    void onLoveTriggered( bool loved );
    void onBanClicked();
    void onBanFinished();

    void onTuningIn( const RadioStation& station );
    void onTrackStarted( const Track& track, const Track& oldTrack );
    void onError( int error , const QVariant& errorData );
    void onStopped();

    void onTick( qint64 );

private:
    void setIconForRadio( const RadioStation& station );

private:
    Ui::PlaybackControlsWidget *ui;

    QPointer<QAction> m_playAction;

    bool m_scrobbleTrack;
    QPointer<QMovie> m_movie;
};

#endif // PLAYBACKCONTROLS_H
