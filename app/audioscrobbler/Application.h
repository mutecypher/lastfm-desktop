/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <lastfm/global.h>
#include <lastfm/Track>
#include "lib/unicorn/UnicornApplication.h"
#include <QPointer>
#include <QSystemTrayIcon>

class AboutDialog;
class MetadataWindow;
class PlayerConnection;
class PlayerMediator;
class QAction;
class ScrobbleInfoFetcher;
class StopWatch;
class DeviceScrobbler;

#ifdef Q_WS_X11
    class IpodDeviceLinux;
#endif

#if defined(aApp)
#undef aApp
#endif
#define aApp (static_cast<audioscrobbler::Application*>(QCoreApplication::instance()))

namespace audioscrobbler
{
    
    /**
      * @brief Main application logic for the audioscrobbler app.
      *
      * This class contains the core components of the application
      * (ie Audioscrobbler, PlayerConnection etc), top-level gui widgets and the system tray.
      */
    class Application : public unicorn::Application
    {
        Q_OBJECT

        enum State
        {
            Unknown,
            Stopped,
            Paused,
            Playing
        } state;
        
        // we delete these so QPointers
        QPointer<QSystemTrayIcon> m_tray;
        QPointer<Audioscrobbler> m_as;
        QPointer<PlayerMediator> m_mediator;
        QPointer<PlayerConnection> m_connection;
        QPointer<StopWatch> m_watch;
        QPointer<MetadataWindow> m_mw;
        QPointer<ScrobbleInfoFetcher> m_fetcher;
        QPointer<DeviceScrobbler> m_deviceScrobbler;

        Track m_currentTrack;
        Track m_trackToScrobble;

        AboutDialog* m_aboutDialog;
        
        QAction* m_submit_scrobbles_toggle;
        QAction* m_artist_action;
        QAction* m_title_action;
        QAction* m_love_action;
        QAction* m_tag_action;
        QAction* m_share_action;
        QAction* m_show_window_action;
        QAction* m_toggle_window_action;
        QAction* m_scrobble_ipod_action;
        QAction* m_visit_profile_action;
        QAction* m_faq_action;
        QAction* m_forums_action;
        QAction* m_about_action;
        QAction* m_prefs_action;
        
    public:
        Application(int& argc, char** argv);

        void init();
        
        QAction* loveAction() const{ return m_love_action; }
        QAction* tagAction() const{ return m_tag_action; }
        QAction* shareAction() const{ return m_share_action; }
        StopWatch* stopWatch() const;
        PlayerConnection* currentConnection() const;
        DeviceScrobbler* deviceScrobbler() const;
        Track currentTrack() const { return m_currentTrack;}
        ScrobbleInfoFetcher* fetcher() const;
        
    signals:
        void trackStarted( const Track&, const Track& );
        void resumed();
        void paused();
        void stopped();

        void scrobblesCached( const QList<lastfm::Track>& tracks );
        void scrobblesSubmitted( const QList<lastfm::Track>& tracks, int numTracks );

        void lovedStateChanged(bool loved);

        // re-route all the info fetchers singals
        void trackGotInfo(const XmlQuery& lfm);
        void albumGotInfo(const XmlQuery& lfm);
        void artistGotInfo(const XmlQuery& lfm);
        void artistGotEvents(const XmlQuery& lfm);
        void trackGotTopFans(const XmlQuery& lfm);
        void trackGotTags(const XmlQuery& lfm);

        void finished();

    public slots:
        void quit();
        void actuallyQuit();

        void changeLovedState(bool loved);
        void onBusLovedStateChanged(bool);

        void onTrackGotInfo(const XmlQuery& );

    protected:
        virtual void initiateLogin()throw( StubbornUserException );

    private:
        void setTrackInfo();
        void resetTrackInfo();

    private slots:
        void onTrayActivated(QSystemTrayIcon::ActivationReason);
        void onStopWatchTimedOut();
        void setConnection(PlayerConnection*);

        void onCorrected(QString correction);

        void onTagTriggered();
        void onShareTriggered();

        void onVisitProfileTriggered();
        void onFaqTriggered();
        void onForumsTriggered();
        void onAboutTriggered();
        void onPrefsTriggered();

        void onTrackStarted(const Track&, const Track&);
        void onPaused();
        void onResumed();
        void onStopped();

        void onSessionChanged( unicorn::Session* newSession );

        void showWindow();
        void toggleWindow();

        void onMessageReceived(const QStringList& message);
    };
}
