
#include <QAction>
#include <QBuffer>

#include <lastfm/Track.h>

#include "../Application.h"
#include "../Services/RadioService/RadioService.h"

#include "CommandReciever.h"

#import <Cocoa/Cocoa.h>

@interface AirfoilIntegrationSampleAppDelegate : NSObject <NSApplicationDelegate> {
    CommandReciever* m_observer;
    NSData* m_logo;
}

- (AirfoilIntegrationSampleAppDelegate*) init:(CommandReciever*)observer;
- (void) setLogo:(NSData*)logo;
@end

@interface AFAppleScriptPlayPauseCommand: NSScriptCommand { } @end
@interface AFAppleScriptNextCommand: NSScriptCommand { } @end
@interface AFAppleScriptPrevCommand: NSScriptCommand { } @end

@implementation AirfoilIntegrationSampleAppDelegate

- (AirfoilIntegrationSampleAppDelegate*) init:(CommandReciever*)observer
{
    if ( self = [super init] )
    {
        self->m_observer = observer;
    }

    m_logo = nil;

    return self;
}

- (BOOL)application:(NSApplication*)sender delegateHandlesKey:(NSString*)key
{
        return [[NSSet setWithObjects: @"trackTitle", @"artist", @"album", @"duration", @"logo", nil] containsObject:key];
}

- (NSString*)trackTitle
{
    Track track = RadioService::instance().currentTrack();

    if ( !track.isNull() )
    {
        QString title = track.title();
        NSString* nsTitle = [NSString stringWithCharacters:(const unichar *)title.unicode() length:(NSUInteger)title.length() ];
        return nsTitle;
    }

    return nil;
}

- (NSString*)artist
{
    Track track = RadioService::instance().currentTrack();

    if ( !track.isNull() )
    {
        QString artist = RadioService::instance().currentTrack().artist();
        NSString* nsArtist = [NSString stringWithCharacters:(const unichar *)artist.unicode() length:(NSUInteger)artist.length() ];
        return nsArtist;
    }

    return nil;
}

- (NSString*)album
{
    Track track = RadioService::instance().currentTrack();

    if ( !track.isNull() )
    {
        QString album = RadioService::instance().currentTrack().album();
        NSString* nsAlbum = [NSString stringWithCharacters:(const unichar *)album.unicode() length:(NSUInteger)album.length() ];
        return nsAlbum;
    }

    return nil;
}

- (NSNumber*)duration
{
    Track track = RadioService::instance().currentTrack();

    if ( !track.isNull() )
    {
        int duration = RadioService::instance().currentTrack().duration();
        return [NSNumber numberWithInt:duration];
    }

    return nil;
}

- (void)setLogo:(NSData*)logo
{
    m_logo = logo;
}

- (NSData*)logo
{
    m_observer->logo();

    if ( m_logo != nil )
        return m_logo;

    return nil;
}

@end

@implementation AFAppleScriptPlayPauseCommand

- (id)performDefaultImplementation
{
    aApp->playAction()->trigger();
    return nil;
}

@end

@implementation AFAppleScriptNextCommand

- (id)performDefaultImplementation
{
    aApp->skipAction()->trigger();
    return nil;
}

@end

@implementation AFAppleScriptPrevCommand

- (id)performDefaultImplementation
{
    // do nothing for the back button
    return nil;
}

@end

AirfoilIntegrationSampleAppDelegate* g_delegate;

CommandReciever::CommandReciever( QObject *parent )
    :QObject( parent )
{
    g_delegate = [[AirfoilIntegrationSampleAppDelegate alloc] init:this];
    [[NSApplication sharedApplication] setDelegate:(id < NSApplicationDelegate >)g_delegate];
}

void
CommandReciever::logo()
{
    Track currentTrack = RadioService::instance().currentTrack();

    // if the track has changed or stopped playing get rid of the track fetcher
    if ( m_trackImageFetcher )
    {
        if ( m_trackImageFetcher->track() != currentTrack || currentTrack.isNull() )
        {
            delete m_trackImageFetcher;
            [g_delegate setLogo:(NSData*)nil];
        }
    }


    // if we haven't fetched the new track image yet then do it now
    if ( !m_trackImageFetcher && !currentTrack.isNull() )
    {
        m_trackImageFetcher = new TrackImageFetcher( RadioService::instance().currentTrack() );
        connect( m_trackImageFetcher, SIGNAL(finished(QImage)), SLOT(onFinished(QImage)));
        m_trackImageFetcher->startAlbum();
    }
}

void
CommandReciever::onFinished( const class QImage& image )
{
    CGImageRef cgImage = QPixmap::fromImage( image ).toMacCGImageRef();

    NSImage* nsImage = [[NSImage alloc] initWithCGImage:(CGImageRef)cgImage size:(NSSize)NSZeroSize];

    NSData *data = [nsImage TIFFRepresentation];

    [g_delegate setLogo:(NSData*)data];
}

CommandReciever::~CommandReciever()
{
}
