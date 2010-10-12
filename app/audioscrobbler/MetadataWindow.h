#ifndef METADATA_WINDOW_H_
#define METADATA_WINDOW_H_

#include "lib/unicorn/UnicornMainWindow.h"
#include "lib/unicorn/StylableWidget.h"
#include <lastfm/Track>

class QAbstractButton;
class QTabBar;
class QLabel;
class ScrobbleControls;
class FirstRunWizard;
class MessageBar;
class ProfileWidget;
class SlideOverLayout;
class UserToolButton;

class MetadataWindow : public unicorn::MainWindow
{
    Q_OBJECT

    struct{
        class TrackWidget* nowPlaying;
        UserToolButton* userButton;

        MessageBar* message_bar;

        class QSplitter* splitter;
        class RecentTracksWidget* recentTracks;
        QWidget* nowScrobbling;
    } ui;

public:
    MetadataWindow();
    const Track& currentTrack() const{ return m_currentTrack; }

    void addWinThumbBarButton( QAction* );

private slots:
    void onTrackStarted(const Track&, const Track&);
    void onStopped();
    void onPaused();
    void onResumed();

    void onSessionChanged( unicorn::Session* );
    
private:
    Track m_currentTrack;
    QList<QAction*> m_buttons;

    void setCurrentWidget( QWidget* );
    void addWinThumbBarButtons( QList<QAction*>& );

    enum {
        TAB_PROFILE,
        TAB_INFO
    };

};

class TitleBar : public StylableWidget {
Q_OBJECT
public:
    TitleBar(const QString&);

private slots:
    void onConnectionUp();
    void onConnectionDown();
    void onIPodDetected();

signals:
    void closeClicked();

private:
    QLabel* m_inetStatus;
    QLabel* m_iPodStatus;
};

#endif //METADATA_WINDOW_H_

