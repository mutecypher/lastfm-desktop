
#include <QCoreApplication>
#include <QStringList>
#include <QUrl>

#include <qtsparkle/Updater>

#include "Updater.h"

unicorn::Updater::Updater( QWidget* parent )
    :QObject( parent )
{
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
}

void
unicorn::Updater::checkForUpdates()
{
    m_updater->CheckNow();
}

