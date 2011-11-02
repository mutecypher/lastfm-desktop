
#include "Updater.h"

#include <Foundation/Foundation.h>
#include <AppKit/NSNibDeclarations.h>

#include <Sparkle/SUUpdater.h>

SUUpdateDriver* g_updater;

unicorn::Updater::Updater(QObject *parent) :
    QObject(parent)
{
    g_updater = [SUUpdateDriver alloc];
}

void
unicorn::Updater::checkForUpdates()
{
    [g_updater checkForUpdates];
}
