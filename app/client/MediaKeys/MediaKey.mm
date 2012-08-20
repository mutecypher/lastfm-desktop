
#include <QAction>

#include <../Application.h>
#include "MediaKey.h"

#include "lib/3rdparty/SPMediaKeyTap/SPMediaKeyTap.h"

#import <Cocoa/Cocoa.h>


@interface MediaKeyTapDelegate : NSObject {
    SPMediaKeyTap *keyTap;
}
@end

@implementation MediaKeyTapDelegate
+(void)initialize;
{
    if([self class] != [MediaKeyTapDelegate class]) return;

    // Register defaults for the whitelist of apps that want to use media keys
    [[NSUserDefaults standardUserDefaults] registerDefaults:[NSDictionary dictionaryWithObjectsAndKeys:
        [SPMediaKeyTap defaultMediaKeyUserBundleIdentifiers], kMediaKeyUsingBundleIdentifiersDefaultsKey,
    nil]];
}
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    keyTap = [[SPMediaKeyTap alloc] initWithDelegate:self];
    if([SPMediaKeyTap usesGlobalMediaKeyTap])
        [keyTap startWatchingMediaKeys];
    else
        NSLog(@"Media key monitoring disabled");

}

-(void)mediaKeyTap:(SPMediaKeyTap*)keyTap receivedMediaKeyEvent:(NSEvent*)event
{
    NSAssert([event type] == NSSystemDefined && [event subtype] == SPSystemDefinedEventMediaKeys, @"Unexpected NSEvent in mediaKeyTap:receivedMediaKeyEvent:");
    // here be dragons...
    int keyCode = (([event data1] & 0xFFFF0000) >> 16);
    int keyFlags = ([event data1] & 0x0000FFFF);
    BOOL keyIsPressed = (((keyFlags & 0xFF00) >> 8)) == 0xA;
    int keyRepeat = (keyFlags & 0x1);

    if ( keyIsPressed && !keyRepeat )
    {
        QString debugString = QString( "key repeated: %1 " ).arg( keyRepeat );

        switch (keyCode)
        {
            case NX_KEYTYPE_PLAY:
                debugString.append( "Play/pause pressed" );
                aApp->playAction()->trigger();
                break;

            case NX_KEYTYPE_FAST:
            case NX_KEYTYPE_NEXT:
                debugString.append( "Ffwd pressed" );
                aApp->skipAction()->trigger();
                break;

            case NX_KEYTYPE_REWIND:
                debugString.append( "Rewind pressed" );
                break;
            default:
                debugString.append( QString( "Key %1 pressed" ).arg( keyCode ) );
                break;
            // More cases defined in hidsystem/ev_keymap.h
        }

        qDebug() << debugString;
    }
}

@end

MediaKeyTapDelegate* g_tapDelegate;

MediaKey::MediaKey( QObject* parent )
    :QObject( parent )
{
    g_tapDelegate = [MediaKeyTapDelegate alloc];

    connect( aApp->delegate(), SIGNAL(initialize()), SLOT(initialize()) );
    connect( aApp->delegate(), SIGNAL(applicationDidFinishLaunching(void*)), SLOT(applicationDidFinishLaunching(void*)) );
}

bool
MediaKey::macEventFilter( EventHandlerCallRef, EventRef event )
{
    NSEvent* theEvent = reinterpret_cast<NSEvent *>(event);

    // If event tap is not installed, handle events that reach the app instead
    BOOL shouldHandleMediaKeyEventLocally = ![SPMediaKeyTap usesGlobalMediaKeyTap];

    if(shouldHandleMediaKeyEventLocally
            && [theEvent type] == NSSystemDefined
            && [theEvent subtype] == SPSystemDefinedEventMediaKeys)
    {
        [(id)g_tapDelegate mediaKeyTap:nil receivedMediaKeyEvent:theEvent];
    }

    return false;
}

void
MediaKey::initialize()
{
    [g_tapDelegate initialize];
}

void
MediaKey::applicationDidFinishLaunching( void* aNotification )
{
    [g_tapDelegate applicationDidFinishLaunching:static_cast<NSNotification*>(aNotification)];
}

