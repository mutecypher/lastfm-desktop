/*
   Copyright 2004 Gareth Simpson <iscrobbler@xurble.org>
   Copyright 2004-2009 Last.fm Ltd. 

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

#include "main.h"
#include "IPodDetector.h"
#include "Moose.h"
#include "common/c++/logger.h"

#if TARGET_OS_WIN32
    #include "ITunesComThread.h"
    #include "ScrobSubmitter.h"

    ITunesComThread* gCom = 0;
    ScrobSubmitter gSubmitter;
#else
    #include <sys/stat.h>
	#include "ITunesPlaysDatabase.h"
#endif

IPodDetector* gIPodDetector = 0;

static void initLogger();
static bool isClientInstalled();
static void launchClient();
static void setupIPodSystem();
static void cleanup();
static void uninstall();
static void deleteTwiddlyDatabases();
extern void ScrobSubCallback(int reqID, bool error, std::string message, void* userData);
extern void ClearMemory( LogicalAddress dest, SInt32 length );

//-------------------------------------------------------------------------------------------------
//	iTunesPluginMain
//-------------------------------------------------------------------------------------------------
//
#if TARGET_OS_MAC
extern "C" OSStatus iTunesPluginMainMachO( OSType message, PluginMessageInfo * messageInfo, void * refCon )
#else
extern "C" __declspec(dllexport) OSStatus iTunesPluginMain( OSType message, PluginMessageInfo * messageInfo, void * refCon )
#endif
{
	OSStatus		status;
	
	(void) refCon;
	
    switch ( message )
    {
        case kPluginInitMessage:
            initLogger();
            
#if TARGET_OS_MAC
            if ( !isClientInstalled() )
            {
                uninstall();
                return noErr;
            }
#endif
            
            launchClient(); //launches client, on mac
            setupIPodSystem(); //sets up iPod scrobbling
            
#if TARGET_OS_WIN32
            LOG( 3, "Initialising ScrobSubmitter" );
            gSubmitter.Init( "itw", ScrobSubCallback, 0 );
#endif
            
            return RegisterVisualPlugin( messageInfo );
            
            
        case kPluginPrepareToQuitMessage:
            LOG( 3, "EVENT: kPluginPrepareToQuitMessage" );
            
            cleanup();
            return noErr;
            
            
        case kPluginCleanupMessage:
            LOG( 3, "EVENT: kPluginCleanupMessage" );
            
#if TARGET_OS_WIN32
            LOG( 3, "Terminating ScrobSubmitter" );
            gSubmitter.Term();
#endif
            
            return noErr;
            
            
        default:
            return unimpErr;
    }
    
	return status;
}

static void initLogger()
{
#if TARGET_OS_WIN32
    std::wstring path = ScrobSubmitter::GetLogPathW();
    path += L"\\iTunesPlugin.log";
#else
    std::string path = ::getenv( "HOME" );
    path += "/Library/Logs/Last.fm/iTunes Plugin.log";
#endif

    new Logger( path.c_str(), Logger::Debug );
    LOG( 3, "Log file created" );
}


static void launchClient()
{
#if TARGET_OS_MAC
    if ( Moose::launchWithMediaPlayer() )
    {
        std::vector<std::string> args;
        args.push_back( "--tray" );
        Moose::launchAudioscrobbler( args );
    }
#endif
}


static void setupIPodSystem()
{
    if ( !Moose::iPodScrobblingEnabled() )
    {
        deleteTwiddlyDatabases();
        return;
    }

#if TARGET_OS_WIN32
    LOG( 3, "Initialising ITunesComThread" );
    gCom = new ITunesComThread();
#endif

#if TARGET_OS_MAC
    LOG( 3, "Initialising ITunesPlaysDatabase" );
    ITunesPlaysDatabase::init(); // must be done b4 notification center registration

    CFNotificationCenterAddObserver( CFNotificationCenterGetDistributedCenter(),
                                     NULL,
                                     notificationCallback,
                                     CFSTR( "com.apple.iTunes.playerInfo" ),
                                     NULL,
                                     CFNotificationSuspensionBehaviorDeliverImmediately );
#endif

    LOG( 3, "Initialising IPodDetector" );
    gIPodDetector = new IPodDetector;
}


static void cleanup()
{
    LOG( 3, "Deleting IPodDetector" );
    delete gIPodDetector;
    gIPodDetector = 0;
    
#if TARGET_OS_WIN32
    LOG( 3, "Deleting ITunesComThread" );
    delete gCom;
    gCom = 0;
#else
    ITunesPlaysDatabase::finit();
#endif
}


#if TARGET_OS_MAC
static bool isClientInstalled()
{
    struct stat st;
    std::string path = Moose::applicationFolder() + "Last.fm";
    return stat( path.c_str(), &st ) == 0;
}


static void uninstall()
{
    LOG( 3, "Client has gone, uninstalling self." );
    // we have to change directory for the script to work properly
    std::string d = Moose::bundleFolder() + "Contents/Resources";
    // we have to change dir for the script to succeed as a safety precaution
    if (chdir( d.c_str() ) != 0)
        return;
    // always specify paths to tools absolutely! --muesli
    Moose::exec( "/bin/sh", "uninstall.sh" );
}
#endif


static void deleteTwiddlyDatabases()
{
    // we delete the db, or if they ever re-enable iPod scrobbling we will
    // mis-scrobble everything played between now and then
    #if TARGET_OS_WIN32
        //FIXME you prolly should delete the manual iPod databases too
        DeleteFileW( (Moose::applicationSupport() + L"\\iTunesPlays.db").c_str() );
    #else
        std::string script = Moose::bundleFolder() + "Contents/Resources/uninstall.sh";
        Moose::exec( "/bin/sh", "'" + script + "' --db-only" );
    #endif
}

//-------------------------------------------------------------------------------------------------
//	RegisterVisualPlugin
//-------------------------------------------------------------------------------------------------
//
#include <iostream>
OSStatus RegisterVisualPlugin( PluginMessageInfo * messageInfo )
{
    std::cout << "Registering Visual Plugin!!!!" << std::endl;

	PlayerMessageInfo	playerMessageInfo;
	OSStatus			status;
		
	memset( &playerMessageInfo.u.registerVisualPluginMessage, 0, sizeof(playerMessageInfo.u.registerVisualPluginMessage) );

	GetVisualName( playerMessageInfo.u.registerVisualPluginMessage.name );

	SetNumVersion( &playerMessageInfo.u.registerVisualPluginMessage.pluginVersion, kTVisualPluginMajorVersion, kTVisualPluginMinorVersion, kTVisualPluginReleaseStage, kTVisualPluginNonFinalRelease );

	playerMessageInfo.u.registerVisualPluginMessage.options					= GetVisualOptions();
	playerMessageInfo.u.registerVisualPluginMessage.handler					= (VisualPluginProcPtr)VisualPluginHandler;
	playerMessageInfo.u.registerVisualPluginMessage.registerRefCon			= 0;
	playerMessageInfo.u.registerVisualPluginMessage.creator					= kTVisualPluginCreator;
	
	playerMessageInfo.u.registerVisualPluginMessage.pulseRateInHz			= kStoppedPulseRateInHz;	// update my state N times a second
	playerMessageInfo.u.registerVisualPluginMessage.numWaveformChannels		= 2;
	playerMessageInfo.u.registerVisualPluginMessage.numSpectrumChannels		= 2;
	
	playerMessageInfo.u.registerVisualPluginMessage.minWidth				= 64;
	playerMessageInfo.u.registerVisualPluginMessage.minHeight				= 64;
	playerMessageInfo.u.registerVisualPluginMessage.maxWidth				= 0;	// no max width limit
	playerMessageInfo.u.registerVisualPluginMessage.maxHeight				= 0;	// no max height limit
	
	status = PlayerRegisterVisualPlugin( messageInfo->u.initMessage.appCookie, messageInfo->u.initMessage.appProc, &playerMessageInfo );
		
	return status;
}


std::string
GetVersionString()
{
    std::ostringstream os;
    os << kTVisualPluginMajorVersion << "."
       << kTVisualPluginMinorVersion << "."
       << kTVisualPluginReleaseStage << "."
       << kTVisualPluginNonFinalRelease;
    return os.str();
}
