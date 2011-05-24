#ifndef PLAYBACKCONTROLS_H
#define PLAYBACKCONTROLS_H

#include <QWidget>

#include <lastfm/RadioStation>

#include "lib/unicorn/StylableWidget.h"

namespace Ui {
    class PlaybackControlsWidget;
}

class PlaybackControlsWidget : public StylableWidget
{
    Q_OBJECT

public:
    explicit PlaybackControlsWidget(QWidget* parent = 0);
    ~PlaybackControlsWidget();

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
    void onTrackSpooled( const Track& track );
    void onError( int error , const QVariant& errorData );
    void onStopped();

private:
    Ui::PlaybackControlsWidget *ui;
};

#endif // PLAYBACKCONTROLS_H
