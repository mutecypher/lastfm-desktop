#include "MediaKeysApplication.h"
#import <AppKit/AppKit.h>
#import <IOKit/hidsystem/ev_keymap.h>

@interface MediaKeysApp : NSApplication
{
}

@end

@implementation MediaKeysApp

- (void)mediaKeyEvent: (int)key state: (BOOL)state repeat: (BOOL)repeat
{
    switch( key )
    {
        case NX_KEYTYPE_PLAY:
            if( state == 0 )
                NSLog( @"Play Pressed" ); //Play pressed and released
        break;

        case NX_KEYTYPE_NEXT:
            if( state == 0 )
                NSLog( @"Next pressed" );
        break;
        
        case NX_KEYTYPE_FAST:
            if( state == 0 )
                NSLog( @"FF Pressed" ); //Next pressed and released
        break;
        
        case NX_KEYTYPE_REWIND:
            if( state == 0 )
                NSLog( @"RW Pressed" ); //Next pressed and released
        break;
        
        case NX_KEYTYPE_PREVIOUS:
            if( state == 0 )
                NSLog( @"Previous pressed" );
        break;
}
}

- (void)sendEvent: (NSEvent*)event
{
    if( [event type] == NSSystemDefined && [event subtype] == 8 )
    {
        int keyCode = (([event data1] & 0xFFFF0000) >> 16);
        int keyFlags = ([event data1] & 0x0000FFFF);
        int keyState = (((keyFlags & 0xFF00) >> 8)) ==0xA;
        int keyRepeat = (keyFlags & 0x1);
        
        [self mediaKeyEvent: keyCode state: keyState repeat: keyRepeat];
    }

    [super sendEvent: event];
}
@end


void MediaKeysApplication::sharedApplication() {
    [MediaKeysApp sharedApplication];
}
