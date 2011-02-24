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
#include "Application.h"

#include "Widgets/PointyArrow.h"
#include "Widgets/ProfileWidget.h"
#include "Dialogs/SettingsDialog.h"
#include "lib/listener/DBusListener.h"
#include "lib/listener/legacy/LegacyPlayerListener.h"
#include "lib/listener/PlayerConnection.h"
#include "lib/listener/PlayerListener.h"
#include "lib/listener/PlayerMediator.h"
#include "MediaDevices/DeviceScrobbler.h"
#include "MetadataWindow.h"
#include "ScrobbleInfoFetcher.h"
#include "StopWatch.h"
#include "../Widgets/ScrobbleControls.h"

#ifdef Q_WS_MAC
#include "lib/listener/mac/ITunesListener.h"
#endif

#include "lib/unicorn/dialogs/AboutDialog.h"
#include "lib/unicorn/dialogs/ShareDialog.h"
#include "lib/unicorn/dialogs/TagDialog.h"
#include "lib/unicorn/QMessageBoxBuilder.h"
#include "lib/unicorn/UnicornSettings.h"
#include "lib/unicorn/widgets/UserMenu.h"
#include "Wizard/FirstRunWizard.h"

#include <lastfm/Audioscrobbler>
#include <lastfm/XmlQuery>

#include <QRegExp>
#include <QShortcut>
#include <QFileDialog>
#include <QDesktopServices>
#include <QNetworkDiskCache>
#include <QMenu>
#include <QDebug>

#ifdef Q_OS_WIN32
#include "windows.h"
#endif

using audioscrobbler::Application;

#define ELLIPSIS QString::fromUtf8("…")
#define CONTROL_KEY_CHAR QString::fromUtf8("⌃")
#define APPLE_KEY_CHAR QString::fromUtf8("⌘")

#ifdef Q_WS_X11
    #define AS_TRAY_ICON ":16x16.png"
#elif defined( Q_WS_WIN )
    #define AS_TRAY_ICON ":22x22.png"
#elif defined( Q_WS_MAC )
    #define AS_TRAY_ICON ":systray_icon_rest_mac.png"
#endif

Application::Application(int& argc, char** argv) 
            : unicorn::Application(argc, argv),
              m_as( 0 ),
              state( Unknown )
{
    setQuitOnLastWindowClosed( false );
}

void
Application::initiateLogin() throw( StubbornUserException )
{
    if( !unicorn::Settings().value( "FirstRunWizardCompleted", false ).toBool())
    {
        setWizardRunning( true );
        FirstRunWizard w;
        if( w.exec() != QDialog::Accepted ) {
            setWizardRunning( false );
            throw StubbornUserException();
        }
    }
    setWizardRunning( false );

    //this covers the case where the last user was removed
    //and the main window was closed.
    if ( m_mw )
    {
        m_mw->show();
    }

    if ( m_tray )
    {
        //HACK: turns out when all the windows are closed, the tray stops working
        //unless you call the following methods.
        m_tray->hide();
        m_tray->show();
    }

}

void
Application::init()
{
    // Initialise the unicorn base class first!
    unicorn::Application::init();

    QNetworkDiskCache* diskCache = new QNetworkDiskCache(this);
    diskCache->setCacheDirectory( lastfm::dir::cache().path() );
    lastfm::nam()->setCache( diskCache );

/// tray
    m_tray = new QSystemTrayIcon(this);
    QIcon trayIcon( AS_TRAY_ICON );
#ifdef Q_WS_MAC
    trayIcon.addFile( ":systray_icon_pressed_mac.png", QSize(), QIcon::Selected );
#endif

#ifdef Q_WS_WIN
    connect( m_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT( onTrayActivated(QSystemTrayIcon::ActivationReason)) );
#endif

#ifdef Q_WS_X11
    connect( m_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT( onTrayActivated(QSystemTrayIcon::ActivationReason)) );
#endif
    m_tray->setIcon(trayIcon);
    m_tray->show();
    connect( this, SIGNAL( aboutToQuit()), m_tray, SLOT( hide()));

    /// DeviceScrobbler
    m_deviceScrobbler = new DeviceScrobbler;
    connect( m_deviceScrobbler, SIGNAL(foundScrobbles( QList<lastfm::Track>)), this, SIGNAL( foundIPodScrobbles(QList<lastfm::Track>) ));

    /// tray menu
    QMenu* menu = new QMenu;
    (menu->addMenu( new UserMenu()))->setText( "Users");
    m_show_window_action = menu->addAction( tr("Show Scrobbler"));
    m_show_window_action->setShortcut( Qt::CTRL + Qt::META + Qt::Key_S );
    menu->addSeparator();
    m_artist_action = menu->addAction( "" );
    m_title_action = menu->addAction(tr("Ready"));
    m_love_action = menu->addAction(tr("Love"));
    m_love_action->setCheckable( true );
    QIcon loveIcon;
    loveIcon.addFile( ":/love-isloved.png", QSize( 16, 16), QIcon::Normal, QIcon::On );
    loveIcon.addFile( ":/love-rest.png", QSize( 16, 16), QIcon::Normal, QIcon::Off );
    m_love_action->setIcon( loveIcon );
    m_love_action->setEnabled( false );
    connect( m_love_action, SIGNAL(triggered(bool)), SLOT(changeLovedState(bool)));

    m_tag_action = menu->addAction(tr("Tag")+ELLIPSIS);
    m_tag_action->setIcon( QIcon( ":/tag-rest.png" ) );
    m_tag_action->setEnabled( false );
    connect( m_tag_action, SIGNAL(triggered()), SLOT(onTagTriggered()));

    m_share_action = menu->addAction(tr("Share")+ELLIPSIS);
    m_share_action->setIcon( QIcon( ":/share-rest.png" ) );
    m_share_action->setEnabled( false );
    connect( m_share_action, SIGNAL(triggered()), SLOT(onShareTriggered()));

#ifdef Q_WS_X11
    menu->addSeparator();
    m_scrobble_ipod_action = menu->addAction( tr( "Scrobble iPod..." ) );
    connect( m_scrobble_ipod_action, SIGNAL( triggered() ), m_deviceScrobbler, SLOT( onScrobbleIpodTriggered() ) );
#endif

    menu->addSeparator();

    m_visit_profile_action = menu->addAction( tr( "Visit Last.fm profile" ) );
    connect( m_visit_profile_action, SIGNAL( triggered() ), SLOT( onVisitProfileTriggered() ) );

    menu->addSeparator();

    m_submit_scrobbles_toggle = menu->addAction(tr("Submit Scrobbles"));
#ifdef Q_WS_MAC
    m_prefs_action = menu->addAction(tr("Preferences")+ELLIPSIS);
#else
    m_prefs_action = menu->addAction(tr("Options")+ELLIPSIS);
#endif
    connect( m_prefs_action, SIGNAL( triggered() ), this, SLOT( onPrefsTriggered() ) );
    menu->addSeparator();
    QMenu* helpMenu = menu->addMenu( tr( "Help" ) );

    m_faq_action    = helpMenu->addAction( tr( "FAQ" ) );
    m_forums_action = helpMenu->addAction( tr( "Forums" ) );
    m_about_action  = helpMenu->addAction( tr( "About" ) );

    connect( m_faq_action, SIGNAL( triggered() ), SLOT( onFaqTriggered() ) );
    connect( m_forums_action, SIGNAL( triggered() ), SLOT( onForumsTriggered() ) );
    connect( m_about_action, SIGNAL( triggered() ), SLOT( onAboutTriggered() ) );
    menu->addSeparator();

    QAction* quit = menu->addAction(tr("Quit %1").arg( applicationName()));

    connect(quit, SIGNAL(triggered()), SLOT(quit()));

    m_artist_action->setEnabled( false );
    m_title_action->setEnabled( false );
    m_submit_scrobbles_toggle->setCheckable(true);
    m_submit_scrobbles_toggle->setChecked(true);
    m_tray->setContextMenu(menu);

/// MetadataWindow
    m_mw = new MetadataWindow;
    m_mw->addWinThumbBarButton( m_love_action );
    m_mw->addWinThumbBarButton( m_tag_action );
    m_mw->addWinThumbBarButton( m_share_action );

#ifdef Q_WS_MAC
    const int sKeyCode = 1;
#elif defined Q_WS_WIN
    const int sKeyCode = 83;
#endif

#ifndef Q_OS_LINUX
    installHotKey( Qt::ControlModifier | Qt::MetaModifier, sKeyCode, m_toggle_window_action = new QAction( this ), SLOT( trigger()));
#endif
    //although the shortcuts are actually set on the ScrobbleControls widget,
    //setting it here adds the shortkey text to the trayicon menu
    //and it's no problem since, for some reason, the shortcuts don't trigger the actions.
    m_tag_action->setShortcut( Qt::CTRL + Qt::Key_T );
    m_share_action->setShortcut( Qt::CTRL + Qt::Key_S );
    m_love_action->setShortcut( Qt::CTRL + Qt::Key_L );

    // make the love buttons sychronised
    connect(this, SIGNAL(lovedStateChanged(bool)), m_love_action, SLOT(setChecked(bool)));

    // tell the radio that the scrobbler's love state has changed
    connect(this, SIGNAL(lovedStateChanged(bool)), SLOT(sendBusLovedStateChanged(bool)));

    // update the love buttons if love was pressed in the radio
    connect(this, SIGNAL(busLovedStateChanged(bool)), m_love_action, SLOT(setChecked(bool)));
    connect(this, SIGNAL(busLovedStateChanged(bool)), SLOT(onBusLovedStateChanged(bool)));

    // tell everyone that is interested that data about the current track has been fetched
    connect( m_mw, SIGNAL(trackGotInfo(XmlQuery)), SIGNAL(trackGotInfo(XmlQuery)));
    connect( m_mw, SIGNAL(albumGotInfo(XmlQuery)), SIGNAL(albumGotInfo(XmlQuery)));
    connect( m_mw, SIGNAL(artistGotInfo(XmlQuery)), SIGNAL(artistGotInfo(XmlQuery)));
    connect( m_mw, SIGNAL(artistGotEvents(XmlQuery)), SIGNAL(artistGotEvents(XmlQuery)));
    connect( m_mw, SIGNAL(trackGotTopFans(XmlQuery)), SIGNAL(trackGotTopFans(XmlQuery)));
    connect( m_mw, SIGNAL(trackGotTags(XmlQuery)), SIGNAL(trackGotTags(XmlQuery)));
    connect( m_mw, SIGNAL(finished()), SIGNAL(finished()));

    connect( m_mw, SIGNAL(trackGotInfo(XmlQuery)), this, SLOT(onTrackGotInfo(XmlQuery)));

/// mediator
    m_mediator = new PlayerMediator(this);
    connect( m_mediator, SIGNAL(activeConnectionChanged( PlayerConnection* )), SLOT(setConnection( PlayerConnection* )) );

/// listeners
    try{
#ifdef Q_OS_MAC
        ITunesListener* itunes = new ITunesListener(m_mediator);
        connect(itunes, SIGNAL(newConnection(PlayerConnection*)), m_mediator, SLOT(follow(PlayerConnection*)));
        itunes->start();
#endif

        QObject* o = new PlayerListener(m_mediator);
        connect(o, SIGNAL(newConnection(PlayerConnection*)), m_mediator, SLOT(follow(PlayerConnection*)));
        o = new LegacyPlayerListener(m_mediator);
        connect(o, SIGNAL(newConnection(PlayerConnection*)), m_mediator, SLOT(follow(PlayerConnection*)));

#ifdef QT_DBUS_LIB
        DBusListener* dbus = new DBusListener(mediator);
        connect(dbus, SIGNAL(newConnection(PlayerConnection*)), m_mediator, SLOT(follow(PlayerConnection*)));
#endif
    }
    catch(std::runtime_error& e){
        qWarning() << e.what();
        //TODO user visible warning
    }


    connect( m_show_window_action, SIGNAL( triggered()), SLOT( showWindow()), Qt::QueuedConnection );
    connect( m_toggle_window_action, SIGNAL( triggered()), SLOT( toggleWindow()), Qt::QueuedConnection );

    connect( this, SIGNAL(messageReceived(QStringList)), SLOT(onMessageReceived(QStringList)) );
    connect( this, SIGNAL( sessionChanged( unicorn::Session* ) ), SLOT( onSessionChanged( unicorn::Session* ) ) );

    //We're not going to catch the first session change as it happened in the unicorn application before
    //we could connect to the signal!

    if ( !currentSession() )
    {
        QMap<QString, QString> lastSession = unicorn::Session::lastSessionData();
        if ( lastSession.contains( "username" ) && lastSession.contains( "sessionKey" ) )
        {
            changeSession( lastSession[ "username" ], lastSession[ "sessionKey" ] );
        }
    }

    // clicking on a system tray message should show the scrobbler
    connect( m_tray, SIGNAL(messageClicked()), m_show_window_action, SLOT(trigger()));

    // Do this last so that when the user logs in all the interested widgets find out
    initiateLogin();

    emit messageReceived( arguments() );
}


void
Application::onSessionChanged( unicorn::Session* /*newSession*/ )
{
    disconnect( m_deviceScrobbler, SIGNAL( foundScrobbles( QList<lastfm::Track> )), m_as, SLOT( cache( QList<lastfm::Track> )));

    Audioscrobbler* oldAs = m_as;
    m_as = new Audioscrobbler("ass");
    connect( m_as, SIGNAL(scrobblesCached(QList<lastfm::Track>)), SIGNAL(scrobblesCached(QList<lastfm::Track>)));
    connect( m_deviceScrobbler, SIGNAL( foundScrobbles( QList<lastfm::Track> )), m_as, SLOT( cacheBatch( QList<lastfm::Track> )));
    delete oldAs;

    m_deviceScrobbler->checkCachedIPodScrobbles();
}

void
Application::setConnection(PlayerConnection*c)
{
    if( m_connection ){
        // disconnect from all the objects that we connect to below
        disconnect( m_connection, 0, this, 0);
        disconnect( m_connection, 0, m_mw, 0);
        if(m_watch)
            m_connection->setElapsed(m_watch->elapsed());
    }

    //
    connect(c, SIGNAL(trackStarted(Track, Track)), SLOT(onTrackStarted(Track, Track)));
    connect(c, SIGNAL(paused()), SLOT(onPaused()));
    connect(c, SIGNAL(resumed()), SLOT(onResumed()));
    connect(c, SIGNAL(stopped()), SLOT(onStopped()));

    connect(c, SIGNAL(trackStarted(Track, Track)), SIGNAL(trackStarted(Track, Track)));
    connect(c, SIGNAL(resumed()), SIGNAL(resumed()));
    connect(c, SIGNAL(paused()), SIGNAL(paused()));
    connect(c, SIGNAL(stopped()), SIGNAL(stopped()));
    connect(c, SIGNAL(bootstrapReady(QString)), SIGNAL( bootstrapReady(QString)));

    m_connection = c;

    if(c->state() == Playing || c->state() == Paused){
        c->forceTrackStarted(Track());
    }

    if( c->state() == Paused ) {
        c->forcePaused();
    }
}

void
Application::onTrackStarted(const Track& t, const Track& oldtrack)
{
    // This stops the loving of tracks in the recent tracks list affecting the current track
    disconnect( m_currentTrack.signalProxy(), SIGNAL(loveToggled(bool)), this, SIGNAL(lovedStateChanged(bool)) );

    state = Playing;

    Q_ASSERT(m_connection);

    //TODO move to playerconnection
    if(t.isNull()){
        qWarning() << "Can't start null track!";
        return;
    }

    m_currentTrack = t;

    double trackLengthPercent = unicorn::UserSettings().value( "scrobblePoint", 50 ).toDouble() / 100.0;

    //This is to prevent the next track being scrobbled
    //instead of the track just listened
    if ( trackLengthPercent == 100 && !oldtrack.isNull() )
    {
        m_trackToScrobble = oldtrack;
    }
    else
    {
        m_trackToScrobble = t;
    }

    ScrobblePoint timeout( m_currentTrack.duration() * trackLengthPercent );
    delete m_watch;
    m_watch = new StopWatch(timeout);
    m_watch->start();

    connect( m_watch, SIGNAL(timeout()), SLOT(onStopWatchTimedOut()));
    connect( m_watch, SIGNAL(paused(bool)), SIGNAL(paused(bool)));
    connect( m_watch, SIGNAL(frameChanged( int )), SIGNAL(frameChanged( int )));
    connect( m_watch, SIGNAL(timeout()), SIGNAL(timeout()));

    setTrackInfo();

    qDebug() << "********** AS = " << m_as;
    if( m_as ) {
        m_as->submit();
        qDebug() << "************** Now Playing..";
        m_as->nowPlaying( t );
    }

    connect( t.signalProxy(), SIGNAL(corrected(QString)), SLOT(onCorrected(QString)));
}

void
Application::onTrackGotInfo(const XmlQuery& lfm)
{
    Q_ASSERT(m_connection);
    MutableTrack( m_connection->track() ).setFromLfm( lfm );
}

void
Application::onStopWatchTimedOut()
{
    Q_ASSERT(m_connection);
    if( m_as ) m_as->cache( m_trackToScrobble );
}

void
Application::onPaused()
{
    // We can sometimes get a stopped before a play when the
    // media player is playing before the scrobbler is started
    if ( state == Unknown ) return;

    state = Paused;

    m_currentTrack.removeNowPlaying();

    Q_ASSERT(m_connection);
    Q_ASSERT(m_watch);
    if(m_watch) m_watch->pause();

    //resetTrackInfo();
}

void
Application::onResumed()
{
    // We can sometimes get a stopped before a play when the
    // media player is playing before the scrobbler is started
    if ( state == Unknown ) return;

    state = Playing;

    Q_ASSERT(m_watch);
    Q_ASSERT(m_connection);

    m_currentTrack.updateNowPlaying( m_currentTrack.duration() - (m_watch->elapsed()/1000) );

    if(m_watch) m_watch->resume();

    //setTrackInfo();


}

void
Application::onStopped()
{
    // We can sometimes get a stopped before a play when the
    // media player is playing before the scrobbler is started
    if ( state == Unknown ) return;

    state = Stopped;

    Q_ASSERT(m_watch);
    Q_ASSERT(m_connection);
        
    delete m_watch;
    if( m_as ) m_as->submit();

    resetTrackInfo();
}

void
Application::onCorrected(QString /*correction*/)
{
    setTrackInfo();
}

void
Application::setTrackInfo()
{ 
    QFontMetrics fm( font() );
    QString durationString = " [" + m_currentTrack.durationString() + "]";

    int actionOffsets = fm.width( durationString );
    int actionWidth = m_tray->contextMenu()->actionGeometry( m_artist_action ).width() - actionOffsets;

    QString artistActionText = fm.elidedText( m_currentTrack.artist( lastfm::Track::Corrected ), Qt::ElideRight, actionWidth );
    QString titleActionText = fm.elidedText( m_currentTrack.title( lastfm::Track::Corrected), Qt::ElideRight, actionWidth - fm.width( durationString ) );

    m_artist_action->setText( artistActionText );
    m_artist_action->setToolTip( m_currentTrack.artist( lastfm::Track::Corrected ) );
    m_title_action->setText( titleActionText + durationString );
    m_title_action->setToolTip( m_currentTrack.title( lastfm::Track::Corrected ) + " [" + m_currentTrack.durationString() + "]" );

    m_tray->setToolTip( m_currentTrack.toString() );

    m_love_action->setEnabled( true );
    m_tag_action->setEnabled( true );
    m_share_action->setEnabled( true );

    // make sure that if the love state changes we update all the buttons
    connect( m_currentTrack.signalProxy(), SIGNAL(loveToggled(bool)), SIGNAL(lovedStateChanged(bool)) );
}

void
Application::resetTrackInfo()
{
    m_artist_action->setText( "" );
    m_title_action->setText( tr( "Ready" ));
    m_love_action->setEnabled( false );
    m_tag_action->setEnabled( false );
    m_share_action->setEnabled( false );
}

void 
Application::onTagTriggered()
{
    TagDialog* td = new TagDialog( m_currentTrack, m_mw );
    td->raise();
    td->show();
    td->activateWindow();
}

void 
Application::onShareTriggered()
{
    ShareDialog* sd = new ShareDialog( m_currentTrack, m_mw );
    sd->raise();
    sd->show();
    sd->activateWindow();
}

void
Application::onVisitProfileTriggered()
{
    QDesktopServices::openUrl( User().www() );
}


void
Application::onFaqTriggered()
{
    QDesktopServices::openUrl( "http://" + tr( "www.last.fm" ) + "/help/faq/" );
}

void
Application::onForumsTriggered()
{
    QDesktopServices::openUrl( "http://" + tr( "www.last.fm" ) + "/forum/34905/" );
}

void
Application::onAboutTriggered()
{
    if ( m_aboutDialog ) m_aboutDialog = new AboutDialog( m_mw );
    m_aboutDialog->show();
}

void
Application::onPrefsTriggered()
{
    SettingsDialog* settingsDialog = new SettingsDialog();
    settingsDialog->exec();
}

void 
Application::changeLovedState(bool loved)
{
    MutableTrack track( m_currentTrack );

    if (loved)
        track.love();
    else
        track.unlove();
}

void
Application::onBusLovedStateChanged( bool loved )
{
    MutableTrack( m_currentTrack ).setLoved( loved );
}

PlayerConnection*
Application::currentConnection() const
{
    return m_connection;
}

StopWatch* 
Application::stopWatch() const
{
    return m_watch;
}

DeviceScrobbler* 
Application::deviceScrobbler() const
{
    return m_deviceScrobbler.data();
}

void 
Application::onTrayActivated( QSystemTrayIcon::ActivationReason reason ) 
{
    if( reason == QSystemTrayIcon::Context ) return;
#ifdef Q_WS_WIN
    if( reason != QSystemTrayIcon::DoubleClick ) return;
#endif
    m_show_window_action->trigger();
}

void
Application::showWindow()
{
    m_mw->showNormal();
    m_mw->setFocus();
    m_mw->raise();
    m_mw->activateWindow();
}

void
Application::toggleWindow()
{
    if( activeWindow() )
        m_mw->hide();
    else
        showWindow();
}
    
void
Application::onMessageReceived(const QStringList& message)
{
    qDebug() << "Messages: " << message;

    if ( message.contains( "--twiddly" ))
    {
        m_deviceScrobbler->handleMessage( message );
    }
    else if ( message.contains( "--exit" ) )
    {
        exit();
    }
    else if ( message.contains( "--settings" ) )
    {
        // raise the settings window
        m_prefs_action->trigger();
    }
    else if ( message.contains( "--new-ipod-detected" ) ||
              message.contains( "--ipod-detected" ))
    {
        m_deviceScrobbler->iPodDetected( message );
    }

    if ( !(message.contains( "--tray" ) || message.contains( "--settings" )))
    {
        // raise the app
        m_show_window_action->trigger();
#ifdef Q_OS_WIN32
        SetForegroundWindow(m_mw->winId());
#endif
    }
}


void 
Application::quit()
{
    if( activeWindow())
        activeWindow()->raise();

    if( unicorn::AppSettings().value( "quitDontAsk", false ).toBool()) {
        actuallyQuit();
        return;
    }

    bool dontAsk = false;
    int result = 1;
    if( !unicorn::AppSettings().value( "quitDontAsk", false ).toBool())
      result =
          QMessageBoxBuilder( activeWindow()).setTitle( tr("%1 is about to quit.").arg(applicationName()))
                                             .setText( tr("Tracks played in %1 will not be scrobbled if you continue." )
                                                       .arg( PluginList().availableDescription()) )
                                             .dontAskAgain()
                                             .setIcon( QMessageBox::Question )
                                             .setButtons( QMessageBox::Yes | QMessageBox::No )
                                             .exec(&dontAsk);
    if( result == QMessageBox::Yes )
    {
        unicorn::AppSettings().setValue( "quitDontAsk", dontAsk );
        QCoreApplication::quit();
    }
    
}

void 
Application::actuallyQuit()
{
    QDialog* d = qobject_cast<QDialog*>( sender());
    if( d ) {
        QCheckBox* dontAskCB = d->findChild<QCheckBox*>();
        if( dontAskCB ) {
            unicorn::AppSettings().setValue( "quitDontAsk", ( dontAskCB->checkState() == Qt::Checked ));
        }
    }
    QCoreApplication::quit();
}

