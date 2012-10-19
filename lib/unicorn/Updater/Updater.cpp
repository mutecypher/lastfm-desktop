
#include <QCoreApplication>
#include <QStringList>
#include <QUrl>

#ifndef Q_WS_X11
#include <qtsparkle/Updater>
#endif

#include "Updater.h"

unicorn::Updater::Updater( QWidget* parent )
    :QObject( parent )
{
#ifndef Q_WS_X11
    QString appcast;
#ifdef Q_OS_MAC
    if ( qApp->arguments().contains( "--update" ) )
        appcast = "http://users.last.fm/~michael/updates_mac.xml";
    else if ( qApp->arguments().contains( "--update-static" ) )
        appcast = "http://static.last.fm/client/Mac/updates.xml";
    else
        appcast ="http://cdn.last.fm/client/Mac/updates.xml";
#else

    if ( qApp->arguments().contains( "--update" ) )
        appcast = "http://users.last.fm/~michael/updates_win.xml";
    else if ( qApp->arguments().contains( "--update-static" ) )
        appcast = "http://static.last.fm/client/Win/updates.xml";
    else
        appcast = "http://cdn.last.fm/client/Win/updates.xml";
#endif
    m_updater = new qtsparkle::Updater( appcast, parent );
#endif
}

void
unicorn::Updater::checkForUpdates()
{
#ifndef Q_WS_X11
    m_updater->CheckNow();
#endif
}

