
#ifdef LASTFM_USER_NOTIFICATIONS
#include <Foundation/NSUserNotification.h>
#endif

#include <QPixmap>

#include <Growl/Growl.h>

#include <lastfm/Track.h>

#include "Notify.h"


#ifdef LASTFM_USER_NOTIFICATIONS
@interface MacClickDelegate : NSObject <NSUserNotificationCenterDelegate> {
    unicorn::Notify* m_observer;
}
    - (MacClickDelegate*) initialise:(unicorn::Notify*)observer;
    - (void) userNotificationCenter:(NSUserNotificationCenter *)center didActivateNotification:(NSUserNotification *)notification;
@end

@implementation MacClickDelegate
- (MacClickDelegate*) initialise:(unicorn::Notify*)observer
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
#endif

@interface GrowlClickDelegate : NSObject <GrowlApplicationBridgeDelegate> {
    unicorn::Notify* m_observer;
}
    - (GrowlClickDelegate*) init:(unicorn::Notify*)observer;
    - (void) growlNotificationWasClicked:(id)clickContext;
@end

@implementation GrowlClickDelegate
- (GrowlClickDelegate*) init:(unicorn::Notify*)observer
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
#ifdef LASTFM_USER_NOTIFICATIONS
    if ( [NSUserNotificationCenter class] )
    {
        MacClickDelegate* macDelegate = [[MacClickDelegate alloc] initialise: this];
        [[NSUserNotificationCenter defaultUserNotificationCenter] setDelegate:macDelegate];
        [[NSUserNotificationCenter defaultUserNotificationCenter] removeAllDeliveredNotifications];
    }
    else
#endif
    {
        GrowlClickDelegate* growlDelegate = [[GrowlClickDelegate alloc] init: this];
        [GrowlApplicationBridge setGrowlDelegate:growlDelegate];
    }
}

unicorn::Notify::~Notify()
{
#ifdef LASTFM_USER_NOTIFICATIONS
    if ( [NSUserNotificationCenter class] )
    {
        [[NSUserNotificationCenter defaultUserNotificationCenter] removeAllDeliveredNotifications];
    }
#endif
}

void
unicorn::Notify::newTrack( const lastfm::Track& track )
{
    delete m_trackImageFetcher;
    m_trackImageFetcher = new TrackImageFetcher( track, Track::LargeImage );
    connect( m_trackImageFetcher, SIGNAL(finished(QPixmap)), SLOT(onFinished(QPixmap)) );

#ifdef LASTFM_USER_NOTIFICATIONS
    if ( [NSUserNotificationCenter class] )
    {
        [[NSUserNotificationCenter defaultUserNotificationCenter] removeAllDeliveredNotifications];
        onFinished( QPixmap() );
    }
    else
#endif
    {
        m_trackImageFetcher->startAlbum();
    }
}

void
unicorn::Notify::paused()
{
#ifdef LASTFM_USER_NOTIFICATIONS
    if ( [NSUserNotificationCenter class] )
        [[NSUserNotificationCenter defaultUserNotificationCenter] removeAllDeliveredNotifications];
#endif
}

void
unicorn::Notify::resumed()
{
#ifdef LASTFM_USER_NOTIFICATIONS
    if ( [NSUserNotificationCenter class] )
    {
        if ( m_trackImageFetcher )
        {
            [[NSUserNotificationCenter defaultUserNotificationCenter] removeAllDeliveredNotifications];
            onFinished( QPixmap() );
        }
    }
#endif
}

void
unicorn::Notify::stopped()
{
#ifdef LASTFM_USER_NOTIFICATIONS
    if ( [NSUserNotificationCenter class] )
    {
        [[NSUserNotificationCenter defaultUserNotificationCenter] removeAllDeliveredNotifications];
        delete m_trackImageFetcher;
    }
#endif
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

#ifdef LASTFM_USER_NOTIFICATIONS
    if ( [NSUserNotificationCenter class] )
    {
        NSUserNotification* userNotification = [NSUserNotification alloc];

        [userNotification setTitle:nsTitle];
        [userNotification setSubtitle:nsDescription];

        [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification:userNotification];
    }
    else
#endif
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
