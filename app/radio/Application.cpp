/*
   Copyright 2009 Last.fm Ltd. 
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
#include "lib/unicorn/QMessageBoxBuilder.h"
#include "Radio.h"
#include "ScrobSocket.h"
#include <QDebug>
#include <QProcess>
#include <QShortcut>
#ifdef Q_OS_MAC
    #include <CoreServices/CoreServices.h>
#endif
using moralistfad::Application;


Application::Application( int& argc, char** argv ) 
    : unicorn::Application( argc, argv )
{
}

void
Application::init()
{
    unicorn::Application::init();

    setQuitOnLastWindowClosed( true );

    initiateLogin();

#ifdef Q_OS_MAC
    FSRef appRef;
    LSFindApplicationForInfo( kLSUnknownCreator, CFSTR( "fm.last.audioscrobbler" ), NULL, &appRef, NULL );

    AEDesc desc;
    const char* data = "tray";
    AECreateDesc( typeChar, data, sizeof( data ), &desc );

    LSApplicationParameters params;
    params.version = 0;
    params.flags = kLSLaunchAndHide;
    params.application = &appRef;
    params.asyncLaunchRefCon = NULL;
    params.environment = NULL;
    CFStringRef arg = CFSTR( "--tray" );
    CFArrayRef args = CFArrayCreate( NULL, ((const void**)&arg), 1, NULL);
    params.argv = args;

    AEAddressDesc target;
    AECreateDesc( typeApplicationBundleID, CFSTR( "fm.last.audioscrobbler" ), 22, &target);

    AppleEvent event;
    AECreateAppleEvent ( kCoreEventClass,
                              kAEReopenApplication ,
                              &target,
                              kAutoGenerateReturnID,
                              kAnyTransactionID,
                              &event );

    AEPutParamDesc( &event, keyAEPropData, &desc );

    params.initialEvent = &event;

    LSOpenApplication( &params, NULL );
    AEDisposeDesc( &desc );
#elif defined Q_OS_WIN
    QProcess* process = new QProcess;
    process->start( QApplication::applicationDirPath() + "/Last.fm Scrobbler.exe", QStringList("--tray") );
#endif
}


// lastfmlib invokes this directly, for some errors:
void
Application::onWsError( lastfm::ws::Error e )
{
    switch (e)
    {
        case lastfm::ws::InvalidSessionKey:
            quit();
            break;
        default:
            break;
    }
}

enum Argument
{
    LastFmUrl,
    Pause, //toggles pause
    Skip,
    Exit,
    Unknown
};

Argument argument( const QString& arg )
{
    if (arg == "--pause") return Pause;
    if (arg == "--skip") return Skip;
    if (arg == "--exit") return Exit;

    QUrl url( arg );
    //TODO show error if invalid schema and that
    if (url.isValid() && url.scheme() == "lastfm") return LastFmUrl;

    return Unknown;
}

void
Application::onMessageReceived( const QStringList& message )
{
    parseArguments( message );
}

void
Application::parseArguments( const QStringList& args )
{
    qDebug() << args;

    if (args.size() <= 1)
        return;

    foreach (QString const arg, args.mid( 1 ))
        switch (argument( arg ))
        {
            case LastFmUrl:
                radio->play( RadioStation( arg ) );
                break;

            case Exit:
                exit();
                break;

            case Skip:
                radio->skip();
                break;

            case Pause:
                if ( radio->state() == Radio::Playing )
                    radio->pause();
                else if ( radio->state() == Radio::Paused )
                    radio->resume();
                break;

            case Unknown:
                qDebug() << "Unknown argument:" << arg;
                break;
        }
}
