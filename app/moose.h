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
#ifndef MOOSE_H
#define MOOSE_H

#include <QProcess>

#include "lib/unicorn/UnicornSettings.h"
#include <core/misc.h>
#ifdef Q_OS_MAC
    #define AUDIOSCROBBLER_BUNDLEID "fm.last.Last.fm"
    #include <ApplicationServices/ApplicationServices.h>

    //This macro clashes with Qt headers
    #undef check
#endif


namespace moose
{
#ifdef WIN32
    struct HklmSettings : QSettings
    {
        HklmSettings() : QSettings( "HKEY_LOCAL_MACHINE\\Software\\Last.fm\\Client", QSettings::NativeFormat )
        {}
    };
#endif


    static inline const char* id() { return "Lastfm-F396D8C8-9595-4f48-A319-48DCB827AD8F"; }
    /** passed to QCoreApplication::setApplicationName() */
    static inline const char* applicationName() { return "Last.fm"; }

    static inline QString path()
    {
        #ifdef __APPLE__
            return "/Applications/Last.fm.app/Contents/MacOS/Last.fm";
        #endif
        #ifdef WIN32
            QString path = unicorn::Settings().value( "Path" ).toString();
            if (path.size())
                return path;

            path = HklmSettings().value( "Path" ).toString();
            if (path.size())
                return path;

            return lastfm::dir::programFiles().filePath( "Last.fm/Last.fm.exe" );
        #endif
    }

    static inline void startAudioscrobbler( QStringList& vargs )
    {
#ifndef Q_OS_MAC
        QProcess::startDetached( moose::path(), vargs);
#elif defined Q_OS_MAC
        FSRef appRef;
        LSFindApplicationForInfo( kLSUnknownCreator, CFSTR( AUDIOSCROBBLER_BUNDLEID ), NULL, &appRef, NULL );

        const void* arg[vargs.size()];

        int index(0);

        AEDescList argAEList;
        AECreateList( NULL, 0, FALSE, &argAEList );
        
        foreach( QString i, vargs ) {
            arg[index++] = CFStringCreateWithCString( NULL, i.toUtf8().data(), kCFStringEncodingUTF8 );
            AEPutPtr( &argAEList, 0, typeChar, i.toUtf8().data(), i.toUtf8().length());
        }

        LSApplicationParameters params;
        params.version = 0;
        params.flags = kLSLaunchAndHide;
        params.application = &appRef;
        params.asyncLaunchRefCon = NULL;
        params.environment = NULL;

        CFArrayRef args = CFArrayCreate( NULL, ((const void**)arg), vargs.size(), NULL);
        params.argv = args;


        AEAddressDesc target;
        AECreateDesc( typeApplicationBundleID, CFSTR( AUDIOSCROBBLER_BUNDLEID ), 16, &target);

        AppleEvent event;
        AECreateAppleEvent ( kCoreEventClass,
                kAEReopenApplication ,
                &target,
                kAutoGenerateReturnID,
                kAnyTransactionID,
                &event );

        AEPutParamDesc( &event, keyAEPropData, &argAEList );

        params.initialEvent = &event;

        LSOpenApplication( &params, NULL );
        //AEDisposeDesc( &argAEList );
        //AEDisposeDesc( &target );

#endif
    }

    static inline QDir dir()
    {
        return QFileInfo( path() ).absoluteDir();
    }



    enum ItemDataRole
    {
        TrackRole = Qt::UserRole,
        TimestampRole,
        CumulativeCountRole,
        SecondaryDisplayRole,
        SmallDisplayRole,
        UrlRole,
        WeightingRole,
        IdentityRole,
        TypeRole,
        HighlightRole
    };
}

#endif
