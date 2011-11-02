
#include "Updater.h"

#include <Foundation/Foundation.h>
#include <AppKit/NSNibDeclarations.h>

#include <Sparkle/Sparkle.h>

unicorn::Updater::Updater(QObject *parent) :
    QObject(parent)
{
    [[SUUpdater sharedUpdater] checkForUpdatesInBackground];
}

void
unicorn::Updater::checkForUpdates()
{
    [[SUUpdater sharedUpdater] checkForUpdates:0];
}
