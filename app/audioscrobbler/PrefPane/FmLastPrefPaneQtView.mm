#import "FmLastPrefPaneQtView.h"
#include "FmLastPrefPanePrefWidget.h"

@implementation FmLastPrefPaneQtView

- (id) initWithFrame:(NSRect)frameRect {
    [super initWithFrame: frameRect];
    
    pw = new FmLastPrefPanePrefWidget;
    NSView* pwView = pw->view();
    [self addSubview: pwView positioned:NSWindowAbove relativeTo:nil];
//    [pwView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
//    [pwView setAutoresizesSubviews:YES];

    [self drawRect: [self frame]];
    return self;
}

- (void) drawRect:(NSRect)rect {
    NSView* pwView = pw->view();
    [pwView setFrameOrigin: NSMakePoint( 0, [self frame].size.height - [pwView frame].size.height)];
    [pwView setFrameSize: [self frame].size];    
    
    [super drawRect: rect];
}

- (FmLastPrefPanePrefWidget*) prefWidget {
    return pw;
}

@end
