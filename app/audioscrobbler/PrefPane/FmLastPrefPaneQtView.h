#import <Cocoa/Cocoa.h>

@interface FmLastPrefPaneQtView : NSView {
    class FmLastPrefPanePrefWidget* pw;
}

- (id) initWithFrame:(NSRect)frameRect;
- (void) drawRect:(NSRect)rect;
@end
