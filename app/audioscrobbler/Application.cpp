
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
#include "MetadataWindow.h"
#include "ScrobbleControls.h"
#include "StopWatch.h"
#include "lib/listener/DBusListener.h"
#include "lib/listener/PlayerConnection.h"
#include "lib/listener/PlayerListener.h"
#include "lib/listener/PlayerMediator.h"
#include "lib/listener/legacy/LegacyPlayerListener.h"
#ifdef Q_WS_MAC
#include "lib/listener/mac/ITunesListener.h"
#endif
#include <lastfm/Audioscrobbler>
#include <QMenu>
#include <QDebug>
#include "lib/unicorn/dialogs/TagDialog.h"
#include "lib/unicorn/dialogs/ShareDialog.h"
#include "lib/unicorn/UnicornSettings.h"
#include "Wizard/FirstRunWizard.h"

#ifdef Q_OS_WIN32
#include "windows.h"
#endif

using audioscrobbler::Application;

#define ELLIPSIS QString::fromUtf8("…")

#ifdef Q_WS_X11
    #define AS_TRAY_ICON ":16x16.png"
#elif defined( Q_WS_WIN )
    #define AS_TRAY_ICON ":22x22.png"
#elif defined( Q_WS_MAC )
    #define AS_TRAY_ICON ":systray_icon_rest_mac.png"
#endif

Application::Application(int& argc, char** argv) 
            : unicorn::Application(argc, argv),
              as( 0 )
{
	setQuitOnLastWindowClosed( false );
    // We do the actual init slightly later so that if this is the second
    // time we open the app, we don't get another tray icon etc.
    QTimer::singleShot(0, this, SLOT(init()));

}

void
Application::init()
{
    if( !unicorn::Settings().value( "FirstRunWizardCompleted", false ).toBool())
    {
        FirstRunWizard* w = new FirstRunWizard();
        if( !w->exec() ) {
            quit();
            return;
        }
    }
    
    as = new Audioscrobbler("ass");
/// tray
    tray = new QSystemTrayIcon(this);
    QIcon trayIcon( AS_TRAY_ICON );
    #ifdef Q_WS_MAC
        trayIcon.addFile( ":systray_icon_pressed_mac.png", QSize(), QIcon::Selected );
    #endif

    #ifdef Q_WS_WIN
        connect( tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT( onTrayActivated(QSystemTrayIcon::ActivationReason)) );
    #endif
    tray->setIcon(trayIcon);
    tray->show();
    connect( this, SIGNAL( aboutToQuit()), tray, SLOT( hide()));

/// tray menu
    QMenu* menu = new QMenu;
    m_toggle_window_action = menu->addAction( tr("Show Scrobbler"));
    menu->addSeparator();
    m_title_action = menu->addAction(tr("Ready"));
    m_love_action = menu->addAction(tr("Love"));
    connect( m_love_action, SIGNAL(triggered()), SLOT(onLoveTriggered()));
    m_tag_action = menu->addAction(tr("Tag")+ELLIPSIS);
    connect( m_tag_action, SIGNAL(triggered()), SLOT(onTagTriggered()));
    m_share_action = menu->addAction(tr("Share")+ELLIPSIS);
    connect( m_share_action, SIGNAL(triggered()), SLOT(onShareTriggered()));
    menu->addSeparator();
    m_submit_scrobbles_toggle = menu->addAction(tr("Submit Scrobbles"));
#ifdef Q_WS_MAC
    menu->addAction(tr("Preferences")+ELLIPSIS);
#else
    menu->addAction(tr("Options")+ELLIPSIS);
#endif
    menu->addSeparator();
    QAction* quit = menu->addAction(tr("Quit Audioscrobbler"));

    connect(quit, SIGNAL(triggered()), SLOT(quit()));

    m_title_action->setEnabled( false );
    m_submit_scrobbles_toggle->setCheckable(true);
    m_submit_scrobbles_toggle->setChecked(true);
    tray->setContextMenu(menu);

/// MetadataWindow
    mw = new MetadataWindow;
    ScrobbleControls* sc = mw->scrobbleControls();
    sc->setEnabled( false );
    sc->setLoveAction( m_love_action );
    sc->setTagAction( m_tag_action );
    sc->setShareAction( m_share_action );



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


    connect( m_toggle_window_action, SIGNAL( triggered()), SLOT( onActivateWindow()), Qt::QueuedConnection );

    connect( this, SIGNAL(messageReceived(QString)), SLOT(onMessageReceived(QString)) );
    connect( this, SIGNAL( sessionChanged( unicorn::Session, unicorn::Session) ), 
                   SLOT(onSessionChanged()));

    if (!arguments().contains("--tray"))
    {
        m_toggle_window_action->trigger();
    }

    // clicking on a system tray message should show the scrobbler
    connect( tray, SIGNAL(messageClicked()), m_toggle_window_action, SLOT(trigger()));


}


void
Application::onSessionChanged()
{
    Audioscrobbler* oldAs = as;
    as = new Audioscrobbler("ass");
    delete oldAs;
}

void
Application::setConnection(PlayerConnection*c)
{
    if(connection){
        disconnect(connection, 0, this, 0);
        disconnect(connection, 0, mw, 0);
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
    connection = c;

    if(c->state() == Playing || c->state() == Paused){
        onTrackStarted(c->track(), Track());
        mw->onTrackStarted(c->track(), Track());
    }

    if( c->state() == Paused ) {
        onPaused();
        mw->onPaused();
    }
}

void
Application::onTrackStarted(const Track& t, const Track& oldtrack)
{
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
    Q_ASSERT(connection);
    Q_ASSERT(watch);
    if(watch) watch->pause();
}

void
Application::onResumed()
{
    Q_ASSERT(watch);
    Q_ASSERT(connection);    

    if(watch) watch->resume();
}

void
Application::onStopped()
{
    Q_ASSERT(watch);
    Q_ASSERT(connection);
        
    delete watch;
   if( as ) as->submit();

   mw->scrobbleControls()->setEnabled( false );
}

void 
Application::onTagTriggered()
{
    TagDialog* td = new TagDialog( mw->currentTrack(), mw );
    td->show();
}

void 
Application::onShareTriggered()
{
    ShareDialog* sd = new ShareDialog( mw->currentTrack(), mw );
    sd->show();
}

void 
Application::onLoveTriggered()
{
    MutableTrack t( mw->currentTrack());
    t.love();
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
Application::onActivateWindow()
{
    mw->showNormal();
    mw->setFocus();
    mw->raise();
    mw->activateWindow();
}

void
Application::onMessageReceived(const QString& message)
{
    if (message != "--tray")
    {
        // raise the app
        m_toggle_window_action->trigger();
#ifdef Q_OS_WIN32
		SetForegroundWindow(mw->winId());
#endif
    }
}

