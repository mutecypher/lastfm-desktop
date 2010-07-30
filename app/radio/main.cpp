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
    // first to prevent compilation errors with Qt 4.5.0
    //TODO shorten this mother fucker
    //NOTE including Carbon/Carbon.h breaks things as it has sooo many symbols
    //     in the global namespace
    #include </System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/AE.framework/Versions/A/Headers/AppleEvents.h>
    static pascal OSErr appleEventHandler( const AppleEvent*, AppleEvent*, long );
#endif

#include "_version.h"
#include "Application.h"
#include "ScrobSocket.h"
#include "lib/unicorn/UnicornApplication.h"
#include "lib/unicorn/qtsingleapplication/qtsinglecoreapplication.h"
#include "lib/unicorn/UnicornSettings.h"
#include "MainWindow.h"
#include "Radio.h"

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
    QtSingleCoreApplication::setApplicationName( "Last.fm Radio" );
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
        moralistfad::Application app( argc, argv );

        if ( app.sendMessage( app.arguments().join( "\t" ) ) )
            return 0;

        app.init();

        QObject::connect(&app, SIGNAL(messageReceived(const QString&)), &app, SLOT(onMessageReceived(const QString&)));

		q = new QMainObject;
        radio = new Radio();
        qAddPostRoutine(cleanup);

        ScrobSocket* scrobsock = new ScrobSocket("ass");
        scrobsock->connect(radio, SIGNAL(trackSpooled(Track)), SLOT(start(Track)));
        scrobsock->connect(radio, SIGNAL(stopped()), SLOT(stop()));
        scrobsock->connect(&app, SIGNAL(aboutToQuit()), scrobsock, SLOT(stop()));

      #ifdef Q_WS_MAC
        AEEventHandlerUPP h = NewAEEventHandlerUPP( appleEventHandler );
        AEInstallEventHandler( 'GURL', 'GURL', h, 0, false );
      #endif
        
        MainWindow window;

        app.setActivationWindow( &window );

        q->connect(&window, SIGNAL(startRadio(RadioStation)), SLOT(onStartRadio(RadioStation)));
        window.connect(radio, SIGNAL(error(int, QVariant)), SLOT(onRadioError(int, QVariant)) );

        window.setWindowTitle( app.applicationName() );

        window.show();

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

#ifdef Q_WS_MAC
static pascal OSErr appleEventHandler( const AppleEvent* e, AppleEvent*, long )
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
#endif


void cleanup()
{
    if (radio && radio->audioOutput()) {
	    unicorn::AppSettings().setValue( "Volume", radio->audioOutput()->volume() );
    }
}


#include "main.moc"
