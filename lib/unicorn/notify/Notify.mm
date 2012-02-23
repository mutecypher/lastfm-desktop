
#include <QPixmap>

#include <Growl/Growl.h>

#include <lastfm/Track.h>

#include "Notify.h"



@interface Delegate : NSObject <GrowlApplicationBridgeDelegate> {
    unicorn::Notify* observer;
}
    - (Delegate*) init:(unicorn::Notify*)observer;
    - (void) growlNotificationWasClicked:(id)clickContext;
@end

@implementation Delegate
- (Delegate*) init:(unicorn::Notify*)observer
{
    if ( (self = [super init]) )
    {
        self->observer = observer;
    }

    return self;
}

- (void) growlNotificationWasClicked:(id)clickContext
{
    Q_UNUSED(clickContext)
    self->observer->growlNotificationWasClicked();
}
@end

unicorn::Notify::Notify(QObject *parent) :
    QObject(parent)
{
    Delegate* delegateObject = [[Delegate alloc] init: this];

    [GrowlApplicationBridge setGrowlDelegate:delegateObject];
}

void
unicorn::Notify::newTrack( const lastfm::Track& track )
{
    delete m_trackImageFetcher;
    m_trackImageFetcher = new TrackImageFetcher( track, lastfm::Large );
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


void
unicorn::Notify::growlNotificationWasClicked()
{
    emit clicked();
}
