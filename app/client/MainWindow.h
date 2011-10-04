#ifndef METADATA_WINDOW_H_
#define METADATA_WINDOW_H_

#include <QResizeEvent>

#include <lastfm/Track.h>
#include <lastfm/XmlQuery.h>

#include "lib/unicorn/UnicornMainWindow.h"
#include "lib/unicorn/StylableWidget.h"

#include "Dialogs/PreferencesDialog.h"

using lastfm::XmlQuery;

class QAbstractButton;
class QTabBar;
class QLabel;
class ScrobbleControls;
class FirstRunWizard;
class MessageBar;
class SlideOverLayout;
class UserToolButton;


class MainWindow : public unicorn::MainWindow
{
    Q_OBJECT

    struct{
        class TitleBar* titleBar;
        class StatusBar* statusBar;

        class MessageBar* messageBar;

        class SideBar* sideBar;
        class QStackedWidget* stackedWidget;

        class NowPlayingStackedWidget* nowPlaying;
        class RecentTracksWidget* recentTracks;

        class QScrollArea* profileScrollArea;
        QWidget* profile;
        class QScrollArea* friendsScrollArea;
        QWidget* friends;
        class QScrollArea* radioScrollArea;
        class RadioWidget* radio;

    } ui;

public:
    MainWindow( QMenuBar* );
    const Track& currentTrack() const{ return m_currentTrack; }

    void addWinThumbBarButton( QAction* );

signals:
    void trackGotInfo(XmlQuery);
    void albumGotInfo(XmlQuery);
    void artistGotInfo(XmlQuery);
    void artistGotEvents(XmlQuery);
    void trackGotTopFans(XmlQuery);
    void trackGotTags(XmlQuery);
    void finished();

public slots:
    void onPrefsTriggered();

private slots:
    void onVisitProfile();

    void onTrackStarted(const Track&, const Track&);
    void onStopped();
    void onPaused();
    void onResumed();
    void onTuningIn();

    void onRadioError( int error, const QVariant& data );

    // iPod scrobbling things
    void onIPodDetected( const QString& iPodName );
    void onProcessingScrobbles( const QString& iPodName );
    void onFoundScrobbles( const QList<lastfm::Track>& tracks, const QString& iPodName );
    void onNoScrobblesFound( const QString& iPodName );

private:
    void setCurrentWidget( QWidget* );
    void addWinThumbBarButtons( QList<QAction*>& );
    void setupMenuBar();

    //void resizeEvent( QResizeEvent* event ) { qDebug() << event->size(); }

private:
    Track m_currentTrack;
    class ActivityListItem* m_currentActivity;
    class QDockWidget* m_radioSideBar;
    QList<QAction*> m_buttons;

    QPointer<PreferencesDialog> m_preferences;
};

#endif //METADATA_WINDOW_H_

