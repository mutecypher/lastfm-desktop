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
#ifdef __APPLE__
    // first to prevent compilation errors with Qt 4.5.0-beta1
    #include <Carbon/Carbon.h>
    #include <ApplicationServices/ApplicationServices.h>
    #include <QMainWindow>
    extern void qt_mac_set_menubar_icons( bool );
#elif defined WIN32
	#include <windows.h>
    #include <QAbstractEventDispatcher>
#endif

#include "UnicornApplication.h"
#include "QMessageBoxBuilder.h"
#include "UnicornCoreApplication.h"
#include "dialogs/LoginDialog.h"
#include "dialogs/LoginContinueDialog.h"
#include "dialogs/WelcomeDialog.h"
#include "SignalBlocker.h"
#include "UnicornSettings.h"
#include <lastfm/misc.h>
#include <lastfm/User>
#include <lastfm/ws.h>
#include <lastfm/XmlQuery>
#include <QDir>
#include <QDebug>
#include <QLocale>
#include <QTranslator>
#include <QFile>
#include <QFileInfo>
#include <QTimer>
#include <QDebug>

#include "dialogs/UserManager.h"
unicorn::Application::Application( int& argc, char** argv ) throw( StubbornUserException )
                    : QtSingleApplication( argc, argv ),
                      m_logoutAtQuit( false ),
                      m_signingIn( true )
{
    addLibraryPath(applicationDirPath());

#ifdef Q_WS_MAC
    qt_mac_set_menubar_icons( false );
#endif    
    
    CoreApplication::init();

    setupHotKeys();

#ifdef __APPLE__
    AEEventHandlerUPP h = NewAEEventHandlerUPP( appleEventHandler );
    AEInstallEventHandler( kCoreEventClass, kAEReopenApplication, h, 0, false );
#endif

#ifdef Q_WS_MAC
    #define CSS_PATH "/../Resources/"
#else
    #define CSS_PATH "/"
#endif

    refreshStyleSheet();

    translate();

    connect( &m_bus, SIGNAL( signingInQuery( QString )), SLOT( onSigningInQuery( QString )));
    connect( &m_bus, SIGNAL( sessionQuery( QString )), SLOT( onBusSessionQuery( QString )));
    connect( &m_bus, SIGNAL( sessionChanged( Session )), SLOT( onBusSessionChanged( Session )));
    connect( &m_bus, SIGNAL( lovedStateChanged(bool)), SIGNAL( busLovedStateChanged(bool)));

    m_bus.board();

#ifdef __APPLE__
    setQuitOnLastWindowClosed( false );
#endif

    initiateLogin();

}

void 
unicorn::Application::loadStyleSheet( QFile& file )
{
    file.open( QIODevice::ReadOnly );
    m_styleSheet += file.readAll();
    setStyleSheet( m_styleSheet );
}

void
unicorn::Application::initiateLogin( bool forceLogout ) throw( StubbornUserException )
{
    if( m_bus.isSigningIn() )
    {
        SignalBlocker( &m_bus, SIGNAL( sessionChanged(Session)), -1 ).start();
    } else if( !forceLogout )
    {
        Session busSession = m_bus.getSession();
       
        if( busSession.isValid() )
            m_currentSession = busSession;
    }

    if( !forceLogout && m_currentSession.isValid())
    {
        changeSession( m_currentSession );
    }
    else
    {
        m_signingIn = true;

        while ( m_signingIn )
        {
            LoginDialog d;
            connect( &m_bus, SIGNAL( signingInQuery( QString)), &d, SLOT( raise() ) );

            if ( d.exec() == QDialog::Accepted )
            {
                disconnect( &m_bus, SIGNAL( signingInQuery( QString)), &d, SLOT( raise()));
                LoginContinueDialog lc( d.token() );
                connect( &m_bus, SIGNAL( signingInQuery( QString)), &lc, SLOT( raise() ) );

                lc.raise();

                if ( lc.exec() == QDialog::Accepted )
                {
                    WelcomeDialog( User(lc.session().username())).exec();
                    changeSession( lc.session());

                    m_signingIn = false;
                }
            }
            else
            {
                throw StubbornUserException();
            }
        }
    }
    m_signingIn = false;
   
}


void 
unicorn::Application::manageUsers()
{
    UserManager um;
    connect( &um, SIGNAL( rosterUpdated()), SIGNAL( rosterUpdated()));
    
    if( um.exec())
        changeSession( Session());
}


void
unicorn::Application::translate()
{
#ifdef NDEBUG
    QString const iso3166 = QLocale().name().right( 2 ).toLower();

#ifdef Q_WS_MAC
    QDir const d = lastfm::dir::bundle().filePath( "Contents/Resources/qm" );
#else
    QDir const d = qApp->applicationDirPath() + "/i18n";
#endif

    //TODO need a unicorn/core/etc. translation, plus policy of no translations elsewhere or something!
    QTranslator* t1 = new QTranslator;
    t1->load( d.filePath( "lastfm_" + iso3166 ) );

    QTranslator* t2 = new QTranslator;
    t2->load( d.filePath( "qt_" + iso3166 ) );

    installTranslator( t1 );
    installTranslator( t2 );
#endif
}


unicorn::Application::~Application()
{
    // we do this here, rather than when the setting is changed because if we 
    // did it then the user would be unable to change their mind
    /*
    if (Settings().logOutOnExit() || m_logoutAtQuit)
    {
        AppSettings s;
        s.remove( "SessionKey" );
        s.remove( "Password" );
    }*/
}


void
unicorn::Application::onUserGotInfo()
{
    QNetworkReply* reply = (QNetworkReply*)sender();
    lastfm::UserDetails userInfo( reply );

    const char* key = UserSettings::subscriptionKey();
    Settings().setValue( key, userInfo.isSubscriber() );
    emit gotUserInfo( userInfo );
}


void 
unicorn::Application::onSigningInQuery( const QString& uuid )
{
    qDebug() << "Are we signing in? " << m_signingIn;
    if( m_signingIn )
        m_bus.sendQueryResponse( uuid, "TRUE" );
    else
        m_bus.sendQueryResponse( uuid, "FALSE" );
}


void 
unicorn::Application::onBusSessionQuery( const QString& uuid )
{
    QByteArray ba;
    QDataStream s( &ba, QIODevice::WriteOnly );
    s << currentSession();
    m_bus.sendQueryResponse( uuid, ba );
}


void 
unicorn::Application::onBusSessionChanged( const Session& session )
{
    changeSession( session, false );
}


void 
unicorn::Application::changeSession( const Session& newSession, bool announce )
{
    if( newSession.username() != m_currentSession.username() &&
        Settings().value( "changeSessionConfirmation", true ).toBool()) {
        bool dontAskAgain = false;
        int result = QMessageBoxBuilder( findMainWindow() ).setTitle( tr( "Changing User" ) )
           .setText( tr( "%1 will be logged into the Scrobbler and Last.fm Radio. All music will now be scrobbled to this account. Do you want to continue?" ).arg( newSession.username() ))
           .setIcon( QMessageBox::Information )
           .setButtons( QMessageBox::Yes | QMessageBox::Cancel )
           .dontAskAgain()
           .exec( &dontAskAgain );

        Settings().setValue( "changeSessionConfirmation", !dontAskAgain );
        if( result != QMessageBox::Ok )
            return;
    }
    Session oldSession = currentSession();
    m_currentSession = newSession;
    lastfm::ws::Username = m_currentSession.username();
    lastfm::ws::SessionKey = m_currentSession.sessionKey();
    connect( lastfm::UserDetails::getInfo(), SIGNAL(finished()), this, SLOT(onUserGotInfo()) );
    
    if( announce )
        m_bus.changeSession( currentSession());

    emit sessionChanged( currentSession(), oldSession );
}

void
unicorn::Application::sendBusLovedStateChanged( bool loved )
{
    QByteArray message = loved ? "LOVED=true" : "LOVED=false";
    m_bus.sendMessage(message);
}

void
unicorn::Application::refreshStyleSheet()
{
    m_styleSheet.clear();

    if ( m_cssFileName.isNull() )
    {
        // This is the first time we are loading the stylesheet

        if( !styleSheet().isEmpty() ) {
            m_cssFileName = QDir::currentPath() + QUrl( styleSheet() ).toLocalFile();
            m_cssDir = QFileInfo( m_cssFileName ).path();
        }

        if( styleSheet().isEmpty()) {
            m_cssFileName = applicationDirPath() + CSS_PATH + QFileInfo(applicationFilePath()).baseName() + ".css";
            m_cssDir = applicationDirPath() + CSS_PATH;
        }
    }

    if ( !m_cssFileName.isNull() )
    {
        QFile cssFile( m_cssFileName );
        cssFile.open( QIODevice::ReadOnly );
        m_styleSheet = cssFile.readAll();
        setStyleSheet( m_styleSheet );
        cssFile.close();
    }

    QRegExp rx( "@import\\s*\"([^\"]*)\";" );
    int pos = 0;
    while( (pos = rx.indexIn( m_styleSheet, pos )) != -1 ) {
        QFile f( m_cssDir + "/" +  rx.cap( 1 ));
        loadStyleSheet( f );
        pos += rx.matchedLength();
    }
}

void 
unicorn::Application::installHotKey( Qt::KeyboardModifiers modifiers, quint32 virtualKey, QObject* receiver, const char* slot )
{
    qDebug() << "Installing HotKey";
	quint32 id = m_hotKeyMap.size() + 1;
    m_hotKeyMap[id] = QPair<QObject*, const char*>( receiver, slot );
	
#ifdef __APPLE__
    EventHotKeyID hotKeyID;

    hotKeyID.signature='htk1';
    hotKeyID.id=id;

    UInt32 appleMod=0;
    if( modifiers.testFlag( Qt::ShiftModifier ))
        appleMod |= shiftKey;
    if( modifiers.testFlag( Qt::ControlModifier ))
        appleMod |= controlKey;
    if( modifiers.testFlag( Qt::AltModifier ))
        appleMod |= optionKey;
    if( modifiers.testFlag( Qt::MetaModifier ))
        appleMod |= cmdKey;

    EventHotKeyRef hkRef;

    RegisterEventHotKey( virtualKey, appleMod, hotKeyID, GetApplicationEventTarget(), 0, &hkRef );
#elif defined WIN32
	quint32 winMod = 0;
    if( modifiers.testFlag( Qt::ShiftModifier ))
        winMod |= MOD_SHIFT;
    if( modifiers.testFlag( Qt::ControlModifier ))
        winMod |= MOD_CONTROL;
    if( modifiers.testFlag( Qt::AltModifier ))
        winMod |= MOD_ALT;
    if( modifiers.testFlag( Qt::MetaModifier ))
        winMod |= MOD_WIN;
		
	RegisterHotKey( NULL, id, winMod, virtualKey);
#endif
}


void 
unicorn::Application::setupHotKeys()
{
#ifdef __APPLE__
    EventTypeSpec eventType;
    eventType.eventClass=kEventClassKeyboard;
    eventType.eventKind=kEventHotKeyPressed;

    using unicorn::Application;
    InstallApplicationEventHandler(&Application::hotkeyEventHandler, 1, &eventType, this, NULL);
#elif defined WIN32
    QAbstractEventDispatcher::instance()->setEventFilter( winEventFilter );
#endif
}

void 
unicorn::Application::onHotKeyEvent(quint32 id)
{
    QPair< QObject*, const char*> method = m_hotKeyMap[id];
    QObject* receiver = method.first;
    const char* slot = method.second;
    QTimer::singleShot( 0, receiver, slot );
}

QMainWindow*  
unicorn::Application::findMainWindow()
{
    QMainWindow* ret = 0;
    foreach (QWidget* w, qApp->topLevelWidgets())
        if (ret = qobject_cast<QMainWindow*>(w))
            break;

    return ret;
}

#ifdef __APPLE__
#include <iostream>
OSStatus /* static */
unicorn::Application::hotkeyEventHandler( EventHandlerCallRef, EventRef event, void* data )
{
    unicorn::Application* self = (unicorn::Application*)data;
    EventHotKeyID hkId;
    GetEventParameter( event, kEventParamDirectObject, typeEventHotKeyID, NULL, sizeof(hkId), NULL, &hkId);
    self->onHotKeyEvent( hkId.id );
    return noErr;
}

pascal OSErr /* static */
unicorn::Application::appleEventHandler( const AppleEvent* e, AppleEvent*, long )
{
    OSType id = typeWildCard;
    AEGetAttributePtr( e, keyEventIDAttr, typeType, 0, &id, sizeof(id), 0 );
    
    AEAddressDesc desc;

    OSErr ret;
    ret = AEGetParamDesc( e, keyAEPropData, typeChar, &desc );
    
    if( ret == noErr ) {
        unsigned int size = AEGetDescDataSize( &desc );
        char data[size + 1];
        data[ size ] = 0;
        ret = AEGetDescData( &desc, data, size );
        QString dataString( data );
        if( dataString == "tray" )
            return unimpErr;
    }

    switch (id)
    {
        case kAEQuitApplication:
            qApp->quit();
            return noErr;

        case kAEReopenApplication:
        {
            QMainWindow* mw = qobject_cast<unicorn::Application*>(qApp)->findMainWindow();
            if( mw ) {
                mw->show(), mw->raise(), mw->activateWindow();
            }
            return noErr;
        }

        default:
            return unimpErr;
    }
}
#endif

#ifdef WIN32
bool /* static */
unicorn::Application::winEventFilter ( void* message )
{
	MSG* msg = (MSG*)message;
	if( msg->message == WM_HOTKEY )
	{
		qDebug() << "Filtered WM_HOTKEY";
        qobject_cast<unicorn::Application*>(qApp)->onHotKeyEvent( msg->wParam );
		return true;
	}
	return false;
}
#endif
