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

#include "Utils.h"

#define AUDIOSCROBBLER_BUNDLEID "fm.last.Scrobbler"
#include <ApplicationServices/ApplicationServices.h>

//This macro clashes with Qt headers
#undef check

void
Utils::startAudioscrobbler( QStringList& vargs )
{
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
    params.flags = kLSLaunchAndHide | kLSLaunchDontSwitch | kLSLaunchAsync;
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
}
