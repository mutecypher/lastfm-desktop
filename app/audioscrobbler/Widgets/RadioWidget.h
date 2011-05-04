#ifndef RADIOWIDGET_H
#define RADIOWIDGET_H

#include <lastfm/RadioStation>

#include "lib/unicorn/UnicornMainWindow.h"

namespace Ui { class RadioWidget; }

class RadioWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RadioWidget();
    ~RadioWidget();

signals:
    void aboutToHide();

private slots:
    void onStartClicked();
    void onRadioTick( qint64 tick );
    void onTrackSpooled( const Track& track );
    void onTuningIn( const RadioStation& station );
    void onError( int error , const QVariant& errorData );
    void onStopped();

    void onGotRecentStations();
    void onGotEvents();

    void onLibraryClicked();
    void onMixClicked();
    void onRecomendedClicked();

    void onLoveClicked( bool loved );
    void onLoveTriggered( bool loved );
    void onBanClicked();
    void onBanFinished();
    void onSkipClicked();
    void onInfoClicked();
    void onPlayClicked( bool checked );
    void onPlayTriggered( bool checked );

    void onBackClicked();
    void onNowPlayingClicked();

    void onStationEditTextChanged( const QString& text );

    void onEditClicked();
    void onFilterClicked();

    void onActionsChanged();

    void onSwitch();

    void onSpace();

private:
    Ui::RadioWidget* ui;
};

#endif // RadioWidget_H
