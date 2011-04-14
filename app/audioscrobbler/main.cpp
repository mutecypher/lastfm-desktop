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
#include <QtGlobal>

#ifdef Q_OS_MAC64
    #include <Carbon/Carbon.h>
    static pascal OSErr appleEventHandler( const AppleEvent*, AppleEvent*, void* );
#elif defined Q_OS_MAC32
    #include </System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/AE.framework/Versions/A/Headers/AppleEvents.h>
    static pascal OSErr appleEventHandler( const AppleEvent*, AppleEvent*, long );
#endif

#include <QShortcut>
#include <QKeySequence>
#include <QStringList>
#include <QRegExp>

#include "_version.h"
#include "Application.h"
#include "ScrobSocket.h"
#include "lib/unicorn/UnicornApplication.h"
#include "lib/unicorn/qtsingleapplication/qtsinglecoreapplication.h"
#include "lib/unicorn/UnicornSettings.h"
#include "WindowMain.h"
#include "Radio.h"
#include "app/moose.h"

void cleanup();

class QMainObject : public QObject
{
	Q_OBJECT
	
public slots:
    void onStartRadio(RadioStation rs)
	{
		radio->play( rs );
	}
};

Radio* radio;
QMainObject* q;

namespace lastfm
{
    extern LASTFM_DLLEXPORT QByteArray UserAgent;
}
int main( int argc, char** argv )
{
    QtSingleCoreApplication::setApplicationName( "Last.fm" );
    QtSingleCoreApplication::setApplicationVersion( VERSION );

    // ATTENTION! Under no circumstance change these strings! --mxcl
#ifdef WIN32
    lastfm::UserAgent = "Last.fm Client " VERSION " (Windows)";
#elif __APPLE__
    lastfm::UserAgent = "Last.fm Client " VERSION " (OS X)";
#elif defined (Q_WS_X11)
    lastfm::UserAgent = "Last.fm Client " VERSION " (X11)";
#endif
	
    try
    {
        audioscrobbler::Application app( argc, argv );

        if ( app.sendMessage( app.arguments() ) || app.arguments().contains("--exit") )
            return 0;

        // It's possible that we were unable to send the
        // message, but the app is actually running
        if ( app.isRunning() )
            return 0;

        app.init();
        q = new QMainObject;
        radio = new Radio();
        qAddPostRoutine(cleanup);

        ScrobSocket* scrobsock = new ScrobSocket("ass");
        scrobsock->connect(radio, SIGNAL(trackSpooled(Track)), SLOT(start(Track)));
        scrobsock->connect(radio, SIGNAL(paused()), SLOT(pause()));
        scrobsock->connect(radio, SIGNAL(resumed()), SLOT(resume()));
        scrobsock->connect(radio, SIGNAL(stopped()), SLOT(stop()));
        scrobsock->connect(&app, SIGNAL(aboutToQuit()), scrobsock, SLOT(stop()));

      #ifdef Q_WS_MAC
        AEEventHandlerUPP h = NewAEEventHandlerUPP( appleEventHandler );
        AEInstallEventHandler( 'GURL', 'GURL', h, 0, false );
      #endif
        
        WindowMain windowMain;

        app.setActivationWindow( &windowMain );
        windowMain.setWindowTitle( app.applicationName() );
        windowMain.show();

        app.parseArguments( app.arguments() );

        int result = app.exec();
        scrobsock->stop();
        return result;
    }
    catch (std::exception& e)
    {
        qDebug() << "unhandled exception " << e.what();
    }
    catch (unicorn::Application::StubbornUserException&)
    {
        // user wouldn't log in
        return 0;
    }
}

#ifdef Q_OS_MAC
#ifdef Q_OS_MAC64
static pascal OSErr appleEventHandler( const AppleEvent* e, AppleEvent*, void* )
#elif defined Q_OS_MAC32
static pascal OSErr appleEventHandler( const AppleEvent* e, AppleEvent*, long )
#endif //Q_OS_MAC64/32

{
    OSType id = typeWildCard;
    AEGetAttributePtr( e, keyEventIDAttr, typeType, 0, &id, sizeof(id), 0 );
    
    switch (id)
    {
        case 'GURL':
        {
            DescType type;
            Size size;

            char buf[1024];
            AEGetParamPtr( e, keyDirectObject, typeChar, &type, &buf, 1023, &size );
            buf[size] = '\0';

            radio->play( RadioStation( QString::fromUtf8( buf ) ) );
            return noErr;
        }
            
        default:
            return unimpErr;
    }
}
#endif //Q_OS_MAC


void cleanup()
{
    if (radio && radio->audioOutput()) {
	    unicorn::AppSettings().setValue( "Volume", radio->audioOutput()->volume() );
    }
}


#include "main.moc"
