#ifndef METADATA_WINDOW_H_
#define METADATA_WINDOW_H_

#include "lib/unicorn/UnicornMainWindow.h"
#include "lib/unicorn/StylableWidget.h"
#include <lastfm/Track>
#include <lastfm/XmlQuery>

using lastfm::XmlQuery;

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
        MessageBar* message_bar;

        class QSplitter* splitter;
        QWidget* tracks;
        class TrackWidget* nowPlaying;
        class RecentTracksWidget* recentTracks;
        QWidget* scrobbleInfo;
    } ui;

public:
    MetadataWindow();
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

private slots:
    void onTrackStarted(const Track&, const Track&);
    void onStopped();
    void onPaused();
    void onResumed();

    void onSessionChanged( unicorn::Session* );

    void onTrackClicked( class TrackWidget* );
    
private:
    Track m_currentTrack;
    QList<QAction*> m_buttons;

    void setCurrentWidget( QWidget* );
    void addWinThumbBarButtons( QList<QAction*>& );
    void removeNowPlaying();
    void addNowPlaying( class TrackWidget* trackWidget );

    enum {
        TAB_PROFILE,
        TAB_INFO
    };

};

#endif //METADATA_WINDOW_H_

