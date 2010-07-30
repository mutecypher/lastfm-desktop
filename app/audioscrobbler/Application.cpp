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
#ifdef Q_WS_X11
#include "MediaDevices/IpodDevice_linux.h"
#endif
#include "MetadataWindow.h"
#include "ScrobbleControls.h"
#include "ScrobbleInfoFetcher.h"
#include "StopWatch.h"
#include "SettingsDialog.h"
#include "lib/listener/DBusListener.h"
#include "lib/listener/PlayerConnection.h"
#include "lib/listener/PlayerListener.h"
#include "lib/listener/PlayerMediator.h"
#include "lib/listener/legacy/LegacyPlayerListener.h"
#ifdef Q_WS_MAC
#include "lib/listener/mac/ITunesListener.h"
#endif
#include <lastfm/Audioscrobbler>
#include <lastfm/XmlQuery>
#include <QMenu>
#include <QDebug>
#include "lib/unicorn/dialogs/AboutDialog.h"
#include "lib/unicorn/dialogs/TagDialog.h"
#include "lib/unicorn/dialogs/ShareDialog.h"
#include "lib/unicorn/UnicornSettings.h"
#include "lib/unicorn/QMessageBoxBuilder.h"
#include "lib/unicorn/UnicornSettings.h"
#include "lib/unicorn/widgets/UserMenu.h"
#include "Wizard/FirstRunWizard.h"

#include <QShortcut>
#include <QFileDialog>
#include <QDesktopServices>
#include <QNetworkDiskCache>

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
              as( 0 ),
              state( Unknown )
{
	setQuitOnLastWindowClosed( false );
}

void
Application::init()
{
    // Initialise the unicorn base class first!
    unicorn::Application::init();

    /*if( !unicorn::Settings().value( "FirstRunWizardCompleted", false ).toBool())
    {
        FirstRunWizard* w = new FirstRunWizard();
        if( !w->exec() ) {
            quit();
            return;
        }
    }*/

    QNetworkDiskCache* diskCache = new QNetworkDiskCache(this);
    diskCache->setCacheDirectory( lastfm::dir::cache().path() );
    lastfm::nam()->setCache( diskCache );

/// tray
    tray = new QSystemTrayIcon(this);
    QIcon trayIcon( AS_TRAY_ICON );
#ifdef Q_WS_MAC
    trayIcon.addFile( ":systray_icon_pressed_mac.png", QSize(), QIcon::Selected );
#endif

#ifdef Q_WS_WIN
    connect( tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT( onTrayActivated(QSystemTrayIcon::ActivationReason)) );
#endif

#ifdef Q_WS_X11
    connect( tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT( onTrayActivated(QSystemTrayIcon::ActivationReason)) );
#endif
    tray->setIcon(trayIcon);
    tray->show();
    connect( this, SIGNAL( aboutToQuit()), tray, SLOT( hide()));

/// tray menu
    QMenu* menu = new QMenu;
    (menu->addMenu( new UserMenu()))->setText( "Users");
    m_toggle_window_action = menu->addAction( tr("Show Scrobbler"));
    m_toggle_window_action->setShortcut( Qt::CTRL + Qt::META + Qt::Key_S );
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
    connect( m_scrobble_ipod_action, SIGNAL( triggered() ), SLOT( onScrobbleIpodTriggered() ) );
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

    QAction* quit = menu->addAction(tr("Quit Audioscrobbler"));

    connect(quit, SIGNAL(triggered()), SLOT(quit()));

    m_artist_action->setEnabled( false );
    m_title_action->setEnabled( false );
    m_submit_scrobbles_toggle->setCheckable(true);
    m_submit_scrobbles_toggle->setChecked(true);
    tray->setContextMenu(menu);

/// ScrobbleInfoFetcher
    fetcher = new ScrobbleInfoFetcher;

/// MetadataWindow
    mw = new MetadataWindow;
    mw->addWinThumbBarButton( m_love_action );
    mw->addWinThumbBarButton( m_tag_action );
    mw->addWinThumbBarButton( m_share_action );
#ifdef Q_WS_MAC
    const int sKeyCode = 1;
#elif defined Q_WS_WIN
    const int sKeyCode = 83;
#endif

#ifndef Q_OS_LINUX
    installHotKey( Qt::ControlModifier | Qt::MetaModifier, sKeyCode, m_toggle_window_action, SLOT( trigger()));
#endif
    //although the shortcuts are actually set on the ScrobbleControls widget,
    //setting it here adds the shortkey text to the trayicon menu
    //and it's no problem since, for some reason, the shortcuts don't trigger the actions.
    m_tag_action->setShortcut( Qt::CTRL + Qt::Key_T );
    m_share_action->setShortcut( Qt::CTRL + Qt::Key_S );
    m_love_action->setShortcut( Qt::CTRL + Qt::Key_L );

    ScrobbleControls* sc = mw->scrobbleControls();
    sc->setEnabled( false );
    sc->setTagAction( m_tag_action );
    sc->setShareAction( m_share_action );
    // make the love buttons sychronised
    connect(this, SIGNAL(lovedStateChanged(bool)), m_love_action, SLOT(setChecked(bool)));
    connect(this, SIGNAL(lovedStateChanged(bool)), sc->loveButton(), SLOT(setChecked(bool)));

    // tell the radio that the scrobbler's love state has changed
    connect(this, SIGNAL(lovedStateChanged(bool)), SLOT(sendBusLovedStateChanged(bool)));

    // update the love buttons if love was pressed in the radio
    connect(this, SIGNAL(busLovedStateChanged(bool)), m_love_action, SLOT(setChecked(bool)));
    connect(this, SIGNAL(busLovedStateChanged(bool)), sc->loveButton(), SLOT(setChecked(bool)));

    // tell everyone that is interested that data about the current track has been fetched
    connect(fetcher, SIGNAL(trackGotInfo(XmlQuery)), mw->nowScrobbling(), SLOT(onTrackGotInfo(XmlQuery)));
    connect(fetcher, SIGNAL(trackGotInfo(XmlQuery)), this, SLOT(onTrackGotInfo(XmlQuery)));
    connect(fetcher, SIGNAL(albumGotInfo(XmlQuery)), mw->nowScrobbling(), SLOT(onAlbumGotInfo(XmlQuery)));
    connect(fetcher, SIGNAL(artistGotInfo(XmlQuery)), mw->nowScrobbling(), SLOT(onArtistGotInfo(XmlQuery)));
    connect(fetcher, SIGNAL(trackGotTopFans(XmlQuery)), mw->nowScrobbling(), SLOT(onTrackGotTopFans(XmlQuery)));
    connect(fetcher, SIGNAL(artistGotEvents(XmlQuery)), mw->nowScrobbling(), SLOT(onArtistGotEvents(XmlQuery)));
    connect(fetcher, SIGNAL(trackGotTags(XmlQuery)), mw->nowScrobbling(), SLOT(onTrackGotTags(XmlQuery)));

/// mediator
    mediator = new PlayerMediator(this);
    connect(mediator, SIGNAL(activeConnectionChanged( PlayerConnection* )), SLOT(setConnection( PlayerConnection* )) );

/// listeners
    try{
#ifdef Q_OS_MAC
        ITunesListener* itunes = new ITunesListener(mediator);
        connect(itunes, SIGNAL(newConnection(PlayerConnection*)), mediator, SLOT(follow(PlayerConnection*)));
        itunes->start();
#endif

        QObject* o = new PlayerListener(mediator);
        connect(o, SIGNAL(newConnection(PlayerConnection*)), mediator, SLOT(follow(PlayerConnection*)));
        o = new LegacyPlayerListener(mediator);
        connect(o, SIGNAL(newConnection(PlayerConnection*)), mediator, SLOT(follow(PlayerConnection*)));

#ifdef QT_DBUS_LIB
        DBusListener* dbus = new DBusListener(mediator);
        connect(dbus, SIGNAL(newConnection(PlayerConnection*)), mediator, SLOT(follow(PlayerConnection*)));
#endif
    }
    catch(std::runtime_error& e){
        qWarning() << e.what();
        //TODO user visible warning
    }


    connect( m_toggle_window_action, SIGNAL( triggered()), SLOT( showWindow()), Qt::QueuedConnection );

    connect( this, SIGNAL(messageReceived(QString)), SLOT(onMessageReceived(QString)) );
    connect( this, SIGNAL( sessionChanged( unicorn::Session, unicorn::Session) ), 
                   SLOT(onSessionChanged()));

    //We're not going to catch the first session change as it happened in the unicorn application before
    //we could connect to the signal!
    changeSession( unicorn::Session());

    // clicking on a system tray message should show the scrobbler
    connect( tray, SIGNAL(messageClicked()), m_toggle_window_action, SLOT(trigger()));

    emit messageReceived( arguments().join(";") );
}


void
Application::onSessionChanged()
{
    Audioscrobbler* oldAs = as;
    as = new Audioscrobbler("ass");
    connect( as, SIGNAL(scrobblesSubmitted(QList<lastfm::Track>, int)), SIGNAL(scrobblesSubmitted(QList<lastfm::Track>, int)));
    connect( as, SIGNAL(scrobblesCached(QList<lastfm::Track>)), SIGNAL(scrobblesCached(QList<lastfm::Track>)));
    delete oldAs;
}

void
Application::setConnection(PlayerConnection*c)
{
    if(connection){
        // disconnect from all the objects that we connect to below
        disconnect(connection, 0, this, 0);
        disconnect(connection, 0, mw, 0);
		disconnect(connection, 0, fetcher, 0);
        if(watch)
            connection->setElapsed(watch->elapsed());
    }

    connect(c, SIGNAL(trackStarted(Track, Track)), SLOT(onTrackStarted(Track, Track)));
    connect(c, SIGNAL(paused()), SLOT(onPaused()));
    connect(c, SIGNAL(resumed()), SLOT(onResumed()));
    connect(c, SIGNAL(stopped()), SLOT(onStopped()));
    connect(c, SIGNAL(trackStarted(Track, Track)), mw, SLOT(onTrackStarted(Track, Track)));
    connect(c, SIGNAL(trackStarted(Track, Track)), SIGNAL(trackStarted(Track, Track)));
    connect(c, SIGNAL(resumed()), mw, SLOT(onResumed()));
    connect(c, SIGNAL(paused()), mw, SLOT(onPaused()));
    connect(c, SIGNAL(stopped()), mw, SLOT(onStopped()));
    connect(c, SIGNAL(trackStarted(Track,Track)), fetcher, SLOT(onTrackStarted(Track,Track)));
    connection = c;

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
    state = Playing;

    Q_ASSERT(connection);
    
    //TODO move to playerconnection
    if(t == oldtrack){ 
        qWarning() << "Trying to start the same track as last time, assuming programmer error and doing nothing";
        return;
    }
    if(t.isNull()){
        qWarning() << "Can't start null track!";
        return;
    }
    m_artist_action->setText( t.artist()); 
    m_title_action->setText( t.title() + " [" + t.durationString() + ']' );

    delete watch;
    qDebug() << "********** AS = " << as;
    if( as ) {
        as->submit();
        qDebug() << "************** Now Playing..";
        as->nowPlaying(t);
    }
    ScrobblePoint timeout(t.duration()/2);
    watch = new StopWatch(timeout, connection->elapsed());
    watch->resume();
    connect(watch, SIGNAL(timeout()), SLOT(onStopWatchTimedOut()));

    tray->showMessage(applicationName(), t.toString());

    mw->scrobbleControls()->setEnabled( true );
    m_love_action->setEnabled( true );
    m_tag_action->setEnabled( true );
    m_share_action->setEnabled( true );

    // make sure that if the love state changes we update all the buttons
    connect( t.signalProxy(), SIGNAL(loveToggled(bool)), SIGNAL(lovedStateChanged(bool)) );    
}

void
Application::onTrackGotInfo(const XmlQuery& lfm)
{
    Q_ASSERT(connection);
    MutableTrack( connection->track() ).setFromLfm( lfm );
}

void
Application::onStopWatchTimedOut()
{
    Q_ASSERT(connection);    
    if( as ) as->cache(connection->track());
}

void
Application::onPaused()
{
    // We can sometimes get a stopped before a play when the
    // media player is playing before the scrobbler is started
    if ( state == Unknown ) return;

    state = Paused;

    Q_ASSERT(connection);
    Q_ASSERT(watch);
    if(watch) watch->pause();
}

void
Application::onResumed()
{
    // We can sometimes get a stopped before a play when the
    // media player is playing before the scrobbler is started
    if ( state == Unknown ) return;

    state = Playing;

    Q_ASSERT(watch);
    Q_ASSERT(connection);    

    if(watch) watch->resume();
}

void
Application::onStopped()
{
    // We can sometimes get a stopped before a play when the
    // media player is playing before the scrobbler is started
    if ( state == Unknown ) return;

    state = Stopped;

    Q_ASSERT(watch);
    Q_ASSERT(connection);
        
    delete watch;
    if( as ) as->submit();

    mw->scrobbleControls()->setEnabled( false );
    m_artist_action->setText( "" );
    m_title_action->setText( tr( "Ready" ));
    m_love_action->setEnabled( false );
    m_tag_action->setEnabled( false );
    m_share_action->setEnabled( false );
}

void 
Application::onTagTriggered()
{
    TagDialog* td = new TagDialog( mw->currentTrack(), mw );
    td->raise();
    td->show();
    td->activateWindow();
}

void 
Application::onShareTriggered()
{
    ShareDialog* sd = new ShareDialog( mw->currentTrack(), mw );
    sd->raise();
    sd->show();
    sd->activateWindow();
}

#ifdef Q_WS_X11
void
Application::onScrobbleIpodTriggered()
{
    IpodDevice iPod;
    QString path;

    path = unicorn::UserSettings().value( "device/mountpath", "" ).toString();

    if ( path.isEmpty() || !QFile::exists( path ) )
    {
        path = "";
        QFileDialog dialog( 0, tr( "Where is your iPod mounted?" ), "/" );
        dialog.setOption( QFileDialog::ShowDirsOnly, true );
        dialog.setFileMode( QFileDialog::Directory );

        //The following lines are to make sure the QFileDialog looks native.
        QString backgroundColor( "transparent" );
        dialog.setStyleSheet( "QDockWidget QFrame{ background-color: " + backgroundColor + "; }" );

        if ( dialog.exec() )
        {
            path = dialog.selectedFiles()[ 0 ];
        }

        if ( path.isEmpty() )
            return;

    }

    iPod.setMountPath( path );

    qApp->setOverrideCursor( Qt::WaitCursor );
    QList<Track> tracks = iPod.tracksToScrobble();
    qApp->restoreOverrideCursor();

    qDebug() << tracks.count() << " new tracks to scrobble.";

    if ( tracks.count() )
    {
        as->cache( tracks );
    }
    else if ( !iPod.error().isEmpty() )
    {
        QMessageBoxBuilder( mw )
                .setIcon( QMessageBox::Critical )
                .setTitle( tr( "Scrobble iPod" ) )
                .setText( iPod.error() )
                .exec();
        qDebug() << iPod.error();
    }
    else
    {
        QMessageBoxBuilder( mw )
                .setIcon( QMessageBox::Warning )
                .setTitle( tr( "Scrobble iPod" ) )
                .setText( tr( "No tracks to scrobble since your last sync." ) )
                .exec();
        qDebug() << "No tracks to scrobble";
    }

    //if the iPod mount path was correct we check if we have to update the configuration file
    if ( tracks.count() || iPod.error().isEmpty() )
    {
        if ( unicorn::UserSettings().value( "device/mountpath", "" ).toString() != path )
            unicorn::UserSettings().setValue( "device/mountpath", path );
    }
}
#endif


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
    if ( m_aboutDialog ) m_aboutDialog = new AboutDialog( mw );
    m_aboutDialog->show();
}

void
Application::onPrefsTriggered()
{
    SettingsDialog* settingsDialog = new SettingsDialog( mw );
    settingsDialog->exec();
}

void 
Application::changeLovedState(bool loved)
{
    MutableTrack track( mw->currentTrack() );

    if (loved)
        track.love();
    else
        track.unlove();
}

PlayerConnection*
Application::currentConnection() const
{
    return connection;
}

StopWatch* 
Application::stopWatch() const
{
    return watch;
}

void 
Application::onTrayActivated( QSystemTrayIcon::ActivationReason reason ) 
{
    if( reason == QSystemTrayIcon::Context ) return;
#ifdef Q_WS_WIN
    if( reason != QSystemTrayIcon::DoubleClick ) return;
#endif
    m_toggle_window_action->trigger();
}

void
Application::showWindow()
{
    mw->showNormal();
    mw->setFocus();
    mw->raise();
    mw->activateWindow();
}

void
Application::onMessageReceived(const QString& message)
{
    QStringList arguments = message.split(";");

    int pos = arguments.indexOf( "--twiddled" );

    if ( pos >= 0 )
    {
        QFile iPodScrobblesFile( arguments[ pos + 1 ] );

        if ( iPodScrobblesFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
        {
            QDomDocument iPodScrobblesDoc;
            iPodScrobblesDoc.setContent( &iPodScrobblesFile );
            QDomNodeList tracks = iPodScrobblesDoc.elementsByTagName( "track" );

            QList<lastfm::Track> scrobbles;

            for ( int i(0) ; i < tracks.count() ; ++i )
            {
                lastfm::Track track( tracks.at(i).toElement() );

                int playcount = track.extra("playCount").toInt();

                for ( int j(0) ; j < playcount ; ++j )
                    scrobbles << track;
            }

            // sort the iPod scrobbles before caching them
            if ( scrobbles.count() > 1 )
                qSort ( scrobbles.begin(), scrobbles.end() );

            as->cache( scrobbles );
        }

        iPodScrobblesFile.remove();
    }
    else if ( !arguments.contains( "--tray" ) )
    {
        // raise the app
        m_toggle_window_action->trigger();
#ifdef Q_OS_WIN32
		SetForegroundWindow(mw->winId());
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
    if( result == QMessageBox::Yes ) {
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
