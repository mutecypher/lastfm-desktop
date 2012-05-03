

#include "NothingPlayingWidget.h"

void
NothingPlayingWidget::oniTunesClicked()
{
    // launch iTunes!  
    [[NSWorkspace sharedWorkspace] launchAppWithBundleIdentifier:@"com.apple.iTunes" options:NSWorkspaceLaunchDefault additionalEventParamDescriptor:nil launchIdentifier:NULL];
}

