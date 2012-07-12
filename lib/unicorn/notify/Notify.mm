
#include <Foundation/NSUserNotification.h>

#include <QPixmap>

#include <Growl/Growl.h>

#include <lastfm/Track.h>

#include "Notify.h"


@interface MacDelegate : NSObject <NSUserNotificationCenterDelegate> {
    unicorn::Notify* m_observer;
}
    - (MacDelegate*) init:(unicorn::Notify*)observer;
    - (void) userNotificationCenter:(NSUserNotificationCenter *)center didActivateNotification:(NSUserNotification *)notification;
@end

@implementation MacDelegate
- (MacDelegate*) init:(unicorn::Notify*)observer
{
    if ( (self = [super init]) )
    {
        self->m_observer = observer;
    }

    return self;
}

- (void) userNotificationCenter:(NSUserNotificationCenter *)center didActivateNotification:(NSUserNotification *)notification
{
    Q_UNUSED(center)
    Q_UNUSED(notification)
    self->m_observer->growlNotificationWasClicked();
}
@end

@interface GrowlDelegate : NSObject <GrowlApplicationBridgeDelegate> {
    unicorn::Notify* m_observer;
}
    - (GrowlDelegate*) init:(unicorn::Notify*)observer;
    - (void) growlNotificationWasClicked:(id)clickContext;
@end

@implementation GrowlDelegate
- (GrowlDelegate*) init:(unicorn::Notify*)observer
{
    if ( (self = [super init]) )
    {
        self->m_observer = observer;
    }

    return self;
}

- (void) growlNotificationWasClicked:(id)clickContext
{
    Q_UNUSED(clickContext)
    self->m_observer->growlNotificationWasClicked();
}

@end

unicorn::Notify::Notify(QObject *parent) :
    QObject(parent)
{
    GrowlDelegate* growlDelegate = [[GrowlDelegate alloc] init: this];
    [GrowlApplicationBridge setGrowlDelegate:growlDelegate];

    MacDelegate* macDelegate = [[MacDelegate alloc] init: this];
    [[NSUserNotificationCenter defaultUserNotificationCenter] setDelegate:macDelegate];
}

void
unicorn::Notify::newTrack( const lastfm::Track& track )
{
    delete m_trackImageFetcher;
    m_trackImageFetcher = new TrackImageFetcher( track, Track::LargeImage );
    connect( m_trackImageFetcher, SIGNAL(finished(QPixmap)), SLOT(onFinished(QPixmap)) );
    m_trackImageFetcher->startAlbum();
}

void
unicorn::Notify::onFinished( const QPixmap& pixmap )
{
    Track track = m_trackImageFetcher->track();

    QString title = track.title();
    QString description = tr("%1\n%2").arg( track.artist(), track.album() );

    if ( track.album().isNull() )
        description = track.artist();

    NSString* nsTitle = [NSString stringWithCharacters:(const unichar *)title.unicode() length:(NSUInteger)title.length() ];
    NSString* nsDescription = [NSString stringWithCharacters:(const unichar *)description.unicode() length:(NSUInteger)description.length() ];

    if ( [NSUserNotificationCenter class] )
    {
        NSUserNotification* userNotification = [NSUserNotification alloc];

        [userNotification setTitle:nsTitle];
        [userNotification setSubtitle:nsDescription];

        [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification:userNotification];
    }
    else
    {
        NSData* data = nil;

        if ( !pixmap.isNull() )
        {
            CGImageRef cgImage = pixmap.toMacCGImageRef();
            NSImage* nsImage = [[NSImage alloc] initWithCGImage:(CGImageRef)cgImage size:(NSSize)NSZeroSize];
            data = [nsImage TIFFRepresentation];
        }

        // TODO: Do the growl notification here. It'll be great!
        [GrowlApplicationBridge notifyWithTitle:(NSString *)nsTitle
          description:(NSString *)nsDescription
          notificationName:(NSString *)@"New track"
                                    iconData:(NSData *)data
                                    priority:(signed int)0
                                    isSticky:(BOOL)NO
                                    clickContext:(id)@"context"
                                    identifier:(NSString*)@"identifier" ];
    }

}


void
unicorn::Notify::growlNotificationWasClicked()
{
    emit clicked();
}
