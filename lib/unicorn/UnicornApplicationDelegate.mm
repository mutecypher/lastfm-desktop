
#include "UnicornSettings.h"
#include "UnicornApplicationDelegate.h"

#include <QWidget>
#include <QApplication>
#include <QDebug>

#import <Cocoa/Cocoa.h>
#import <AppKit/NSView.h>

@interface LFMAppDelegate : NSObject <NSApplicationDelegate> {
    unicorn::UnicornApplicationDelegate* m_observer;
    BOOL m_show;
}
- (LFMAppDelegate*) init:(unicorn::UnicornApplicationDelegate*)observer;
- (void)transformStep1:(BOOL)show;
- (void)transformStep2;
- (void)transformStep3;
@end

@implementation LFMAppDelegate

- (void)transformStep1:(BOOL)show;
{
    foreach ( QWidget* widget, QApplication::topLevelWidgets() )
    {
         NSView* view = reinterpret_cast<NSView*>( widget->winId() );
         [[view window] setCanHide:NO];
    }

    m_show = show;
    [[NSRunningApplication currentApplication] activateWithOptions:NSApplicationActivateIgnoringOtherApps];
    [self performSelector:@selector(transformStep2) withObject:nil afterDelay:0.1];
}

- (void)transformStep2
{
    ProcessSerialNumber psn = { 0, kCurrentProcess };
    (void) TransformProcessType(&psn, m_show ? kProcessTransformToForegroundApplication : kProcessTransformToUIElementApplication );
    [self performSelector:@selector(transformStep3) withObject:nil afterDelay:0.1];
}

- (void)transformStep3
{
    [[NSRunningApplication currentApplication] activateWithOptions:NSApplicationActivateIgnoringOtherApps];

    foreach ( QWidget* widget, QApplication::topLevelWidgets() )
    {
         NSView* view = reinterpret_cast<NSView*>( widget->winId() );
         [[view window] setCanHide:YES];
    }
}

- (LFMAppDelegate*) init:(unicorn::UnicornApplicationDelegate*)observer
{
    if ( (self = [super init]) )
    {
        self->m_observer = observer;
    }

    return self;
}

-(void)initialize
{
    m_observer->forceInitialize();
}

- (void)applicationWillFinishLaunching:(NSNotification *)aNotification
{
    qDebug() << "applicationWillFinishLaunching";

    if ( unicorn::Settings().value( "showDock", true ).toBool() )
    {
        ProcessSerialNumber psn = { 0, kCurrentProcess };
        (void) TransformProcessType(&psn, kProcessTransformToForegroundApplication);
    }
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
    if ( m_observer->m_observer )
    {
        QString string = m_observer->m_observer->trackTitle();

        if ( !string.isEmpty() )
            return [NSString stringWithCharacters:(const unichar *)string.unicode() length:(NSUInteger)string.length() ];
    }

    return nil;
}

- (NSString*)artist
{
    if ( m_observer->m_observer )
    {
        QString string = m_observer->m_observer->artist();

        if ( !string.isEmpty() )
            return [NSString stringWithCharacters:(const unichar *)string.unicode() length:(NSUInteger)string.length() ];
    }

    return nil;
}

- (NSString*)album
{
    if ( m_observer->m_observer )
    {
        QString string = m_observer->m_observer->album();

        if ( !string.isEmpty() )
            return [NSString stringWithCharacters:(const unichar *)string.unicode() length:(NSUInteger)string.length() ];
    }

    return nil;
}

- (NSNumber*)duration
{
    return [NSNumber numberWithInt:m_observer->m_observer ? m_observer->m_observer->duration() : 0];
}

- (NSData*)artwork
{
    if ( m_observer->m_observer )
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
    }

    return nil;
}

- (BOOL)loved
{
    return m_observer->m_observer && m_observer->m_observer->loved() ? YES : NO;
}

@end

LFMAppDelegate* g_appDelegate;

unicorn::UnicornApplicationDelegate::UnicornApplicationDelegate(QObject *parent) :
    QObject(parent), m_observer( 0 )
{
    g_appDelegate = [[LFMAppDelegate alloc] init:this];

    [[NSApplication sharedApplication] setDelegate: g_appDelegate];
}

void
unicorn::UnicornApplicationDelegate::showDockIcon( bool show )
{
    [g_appDelegate transformStep1:show?YES:NO];
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

