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
    void onPlayClicked( bool checked );
    void onSkipClicked();
    void onLoveClicked( bool loved );
    void onLoveTriggered();
    void onBanClicked();
    void onBanFinished();
    void onRadioTick( qint64 tick );
    void onTuningIn( const RadioStation& station );
    void onTrackSpooled( const Track& track );

    void onEditClicked();
    void onFilterClicked();

    void onActionsChanged();

    void onSwitch();
    void onSpace();

private:
    void addWinThumbBarButtons( QList<QAction*>& );

private:
    Ui::WindowMini* ui;

    class Actions* m_actions;
};

#endif // WindowMini_H
