#ifndef WindowMini_H
#define WindowMini_H

#include <lastfm/Track>

#include "lib/unicorn/UnicornMainWindow.h"

namespace Ui { class WindowMini; }

class WindowMini : public unicorn::MainWindow
{
    Q_OBJECT

public:
    explicit WindowMini( class Actions& actions );
    ~WindowMini();

signals:
    void aboutToHide();

private slots:
    void onRadioTick( qint64 tick );
    void onTuningIn( const RadioStation& station );
    void onTrackSpooled( const Track& track );
    void onStopped();
    void onError( int errorCode, const QVariant& errorText );

    void onLoveClicked( bool loved );
    void onLoveTriggered( bool loved );
    void onBanClicked();
    void onBanFinished();
    void onInfoClicked();
    void onPlayClicked( bool checked );
    void onPlayTriggered( bool checked );
    void onSkipClicked();

    void onEditClicked();
    void onFilterClicked();

    void onActionsChanged();

    void onSwitch();
    void onSpace();

    void onGotEvents();

private:
    void addWinThumbBarButtons( QList<QAction*>& );

private:
    Ui::WindowMini* ui;

    class Actions* m_actions;
};

#endif // WindowMini_H
