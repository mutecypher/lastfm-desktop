
#include "MediaKey.h"

#include "SPMediaKeyTap.h"
#include "SPMediaKeyTapDelegate.h"

SPMediaKeyTapExampleAppDelegate* g_mediaKeyDelegate;
SPMediaKeyTap* g_mediaKeyTap;

MediaKey::MediaKey( QObject* parent )
    :QObject( parent )
{
    // initialise the delegate thing

    g_mediaKeyDelegate = [SPMediaKeyTapExampleAppDelegate alloc];
    g_mediaKeyTap = [[SPMediaKeyTap alloc] initWithDelegate:(id)g_mediaKeyDelegate];
}

bool
MediaKey::macEventFilter( EventHandlerCallRef, EventRef event )
{
    NSEvent *theEvent = reinterpret_cast<NSEvent *>(event);

    // If event tap is not installed, handle events that reach the app instead
    BOOL shouldHandleMediaKeyEventLocally = ![SPMediaKeyTap usesGlobalMediaKeyTap];

    if(shouldHandleMediaKeyEventLocally && [theEvent type] == NSSystemDefined && [theEvent subtype] == SPSystemDefinedEventMediaKeys)
    {
        [(id)g_mediaKeyDelegate mediaKeyTap:nil receivedMediaKeyEvent:theEvent];

        return true;
    }

    return false;
}
