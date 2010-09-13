#include "UnicornApplication.h"
#import <Foundation/NSAppleEventDescriptor.h>
#import <Foundation/NSAppleEventManager.h>

void 
unicorn::Application::installCocoaEventHandler() const
{
    NSAppleEventManager *appleEventManager = [NSAppleEventManager sharedAppleEventManager];
    [appleEventManager setEventHandler:NULL andSelector:NULL forEventClass:kCoreEventClass andEventID:kAEReopenApplication];
}
