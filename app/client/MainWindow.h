#ifndef METADATA_WINDOW_H_
#define METADATA_WINDOW_H_

#include <QResizeEvent>

#include <lastfm/Track.h>
#include <lastfm/XmlQuery.h>

#include "lib/unicorn/UnicornMainWindow.h"
#include "lib/unicorn/StylableWidget.h"

#ifdef  Q_OS_MAC
#include "Services/ITunesPluginInstaller/ITunesPluginInstaller.h"
#endif

#include "Settings/PreferencesDialog.h"
#include "Dialogs/BetaDialog.h"
#include "Dialogs/DiagnosticsDialog.h"

#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
namespace unicorn { class Updater; }
using unicorn::Updater;
#endif

using lastfm::XmlQuery;

class QAbstractButton;
class QTabBar;
class QLabel;
class ScrobbleControls;
class FirstRunWizard;
class MessageBar;
class SlideOverLayout;
class UserToolButton;
class PluginList;

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
    void onBetaTriggered();
    void onDiagnosticsTriggered();

private slots:
    void onVisitProfile();

    void onTrackStarted(const Track&, const Track&);
    void onStopped();
    void onPaused();
    void onResumed();
    void onTuningIn();

    void onRadioError( int error, const QVariant& data );

    void checkForUpdates();

    void onSpace();

#ifdef Q_OS_WIN32
    void checkUpdatedPlugins();
#endif

    // iPod scrobbling things
    void onIPodDetected( const QString& iPodName );
    void onProcessingScrobbles( const QString& iPodName );
    void onFoundScrobbles( const QList<lastfm::Track>& tracks, const QString& iPodName );
    void onNoScrobblesFound( const QString& iPodName );

private:
    void setCurrentWidget( QWidget* );
    void addWinThumbBarButtons( QList<QAction*>& );
    void setupMenuBar();

    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

    static QString applicationName();

    //void resizeEvent( QResizeEvent* event ) { qDebug() << event->size(); }

private:
    Track m_currentTrack;
    class ActivityListItem* m_currentActivity;
    class QDockWidget* m_radioSideBar;
    QList<QAction*> m_buttons;

    QPointer<PreferencesDialog> m_preferences;
    QPointer<BetaDialog> m_beta;
    QPointer<DiagnosticsDialog> m_diagnostics;
#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
    QPointer<Updater> m_updater;
#endif
    QPointer<PluginList> m_pluginList;

#ifdef Q_WS_MAC
    QPointer<ITunesPluginInstaller> m_installer;
#endif
};

#endif //METADATA_WINDOW_H_

