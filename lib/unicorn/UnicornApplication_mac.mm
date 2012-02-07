#include "UnicornApplication.h"
#import <Foundation/NSAppleEventDescriptor.h>
#import <Foundation/NSAppleEventManager.h>

void 
unicorn::Application::setOpenApplicationEventHandler()
{
    NSAppleEventManager *appleEventManager = [NSAppleEventManager sharedAppleEventManager];
    [appleEventManager setEventHandler:NULL andSelector:NULL forEventClass:kCoreEventClass andEventID:kAEReopenApplication];
}

void
unicorn::Application::setGetURLEventHandler()
{
    NSAppleEventManager *appleEventManager = [NSAppleEventManager sharedAppleEventManager];
    [appleEventManager setEventHandler:NULL andSelector:NULL forEventClass:kInternetEventClass andEventID:kAEGetURL];
}
