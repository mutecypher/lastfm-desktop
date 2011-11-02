
#include "Updater.h"

#include <winsparkle.h>

unicorn::Updater::Updater(QObject *parent) :
    QObject(parent)
{
    win_sparkle_init();
}

void
unicorn::Updater::checkForUpdates()
{
    win_sparkle_check_update_with_ui();
}

unicorn::Updater::~Updater()
{
    win_sparkle_cleanup();
}
