
#include "Updater.h"

#ifdef Q_OS_WIN
#include <qtsparkle/Updater>
#include <QCoreApplication>
#include <QStringList>
#include <QUrl>
#endif //Q_OS_WIN

#if defined(Q_OS_WIN) || defined(Q_WS_X11)
unicorn::Updater::Updater( QWidget *parent )
    :QWidget( parent )
{
#if defined(Q_OS_WIN)
    QUrl appcast;

    if ( qApp->arguments().contains( "--update" ) )
        appcast = "http://users.last.fm/~michael/updates_win.xml";
    else if ( qApp->arguments().contains( "--update-static" ) )
        appcast = "http://static.last.fm/client/Win/updates.xml";
    else
        appcast = "http://cdn.last.fm/client/Win/updates.xml";

    m_updater = new qtsparkle::Updater( appcast, parent );
#endif
}
#endif

#ifndef Q_OS_MAC
void
unicorn::Updater::checkForUpdates()
{
    m_updater->CheckNow();
}

unicorn::Updater::~Updater()
{
}
#endif
