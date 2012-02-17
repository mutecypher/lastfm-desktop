
#include "Updater.h"

#include <Foundation/Foundation.h>
#include <AppKit/NSNibDeclarations.h>

#include <Sparkle/Sparkle.h>

unicorn::Updater::Updater(QObject *parent) :
    QObject(parent)
{
    [SUUpdater sharedUpdater];
}

void
unicorn::Updater::checkForUpdates()
{
    [[SUUpdater sharedUpdater] checkForUpdates:0];
}

unicorn::Updater::~Updater()
{
    // do nothing - included here for header compatibility with windows version
}
