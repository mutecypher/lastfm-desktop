#include "UnicornApplicationDelegate.h"

#import <Cocoa/Cocoa.h>


@interface LFMAppDelegate : NSObject <NSApplicationDelegate> {
    unicorn::UnicornApplicationDelegate* m_observer;
}
- (LFMAppDelegate*) init:(unicorn::UnicornApplicationDelegate*)observer;
@end

@implementation LFMAppDelegate
- (LFMAppDelegate*) init:(unicorn::UnicornApplicationDelegate*)observer
{
    if ( (self = [super init]) )
    {
        self->m_observer = observer;
    }

    return self;
}

-(void)initialize;
{
    m_observer->forceInitialize();
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    m_observer->forceApplicationDidFinishLaunching( aNotification );
}


- (BOOL)application:(NSApplication*)sender delegateHandlesKey:(NSString*)key
{
    Q_UNUSED(sender);
    return [[NSSet setWithObjects: @"trackTitle", @"artist", @"album", @"duration", @"artwork", @"loved", nil] containsObject:key];
}

- (NSString*)trackTitle
{
    QString string = m_observer->m_observer->trackTitle();

    if ( !string.isEmpty() )
        return [NSString stringWithCharacters:(const unichar *)string.unicode() length:(NSUInteger)string.length() ];

    return nil;
}

- (NSString*)artist
{
    QString string = m_observer->m_observer->artist();

    if ( !string.isEmpty() )
        return [NSString stringWithCharacters:(const unichar *)string.unicode() length:(NSUInteger)string.length() ];

    return nil;
}

- (NSString*)album
{
    QString string = m_observer->m_observer->album();

    if ( !string.isEmpty() )
        return [NSString stringWithCharacters:(const unichar *)string.unicode() length:(NSUInteger)string.length() ];

    return nil;
}

- (NSNumber*)duration
{
    return [NSNumber numberWithInt:m_observer->m_observer->duration()];
}

- (NSData*)artwork
{
    QPixmap pixmap = m_observer->m_observer->artwork();

    if ( !pixmap.isNull() )
    {
        CGImageRef cgImage = pixmap.toMacCGImageRef();
        NSImage* nsImage = [[NSImage alloc] initWithCGImage:(CGImageRef)cgImage size:(NSSize)NSZeroSize];
        NSData* data = [nsImage TIFFRepresentation];
        return data;
    }
    else
    {
        NSImage* img = [NSImage imageNamed: NSImageNameApplicationIcon];
        NSData* data = [img TIFFRepresentation];
        return data;
    }

    return nil;
}

- (BOOL)loved
{
    return m_observer->m_observer->loved() ? YES : NO;
}

@end

LFMAppDelegate* g_appDelegate;

unicorn::UnicornApplicationDelegate::UnicornApplicationDelegate(QObject *parent) :
    QObject(parent)
{
    g_appDelegate = [[LFMAppDelegate alloc] init:this];

    [[NSApplication sharedApplication] setDelegate: g_appDelegate];
}

void
unicorn::UnicornApplicationDelegate::setCommandObserver( UnicornApplicationDelegateCommandObserver* observer )
{
    m_observer = observer;
}

void
unicorn::UnicornApplicationDelegate::forceInitialize()
{
    emit initialize();
}

void
unicorn::UnicornApplicationDelegate::forceApplicationDidFinishLaunching( void* aNotification )
{
    emit applicationDidFinishLaunching( aNotification );
}

