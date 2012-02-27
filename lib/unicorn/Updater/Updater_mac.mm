
#include <QCoreApplication>
#include <QStringList>

#include "Updater.h"

#include <AppKit/NSNibDeclarations.h>
#include <Foundation/Foundation.h>
#include <Foundation/NSURL.h>

#include <Sparkle/Sparkle.h>

unicorn::Updater::Updater(QObject *parent) :
    QObject(parent)
{
    SUUpdater* updater = [SUUpdater sharedUpdater];

    if ( qApp->arguments().contains( "--debug" ) )
        [updater setFeedURL:[NSURL URLWithString:@"http://users.last.fm/~michael/updates_mac.xml"]];
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
