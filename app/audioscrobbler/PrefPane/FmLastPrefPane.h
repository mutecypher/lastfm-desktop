#import <Cocoa/Cocoa.h>
#import <PreferencePanes/PreferencePanes.h>
#import "FmLastPrefPaneQtView.h"

@interface FmLastPrefPane : NSPreferencePane {
    IBOutlet FmLastPrefPaneQtView* qtView;
}

- (void)mainViewDidLoad;
- (void)didSelect;
@end
