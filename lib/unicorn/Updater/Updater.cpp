
#include "Updater.h"

#ifdef Q_OS_WIN
#include <winsparkle.h>
#endif

unicorn::Updater::Updater(QObject *parent) :
    QObject(parent)
{
#ifdef Q_OS_WIN
    win_sparkle_init();
#endif
}

void
unicorn::Updater::checkForUpdates()
{
#ifdef Q_OS_WIN
    win_sparkle_check_update_with_ui();
#endif
}

unicorn::Updater::~Updater()
{
#ifdef Q_OS_WIN
    win_sparkle_cleanup();
#endif
}
