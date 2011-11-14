
#include <QDesktopServices>
#include <QCoreApplication>
#include <QUrl>

#include <lastfm/UrlBuilder.h>

#include "DesktopServices.h"

unicorn::DesktopServices::DesktopServices()
{
}


bool
unicorn::DesktopServices::openUrl( QUrl url )
{
    if ( lastfm::UrlBuilder::isHost( url ) )
    {
        url.addQueryItem( "utm_source", "last.fm" );
        url.addQueryItem( "utm_medium", "application" );
        url.addQueryItem( "utm_campaign", "last.fm_desktop_application" );
        url.addQueryItem( "utm_content", QCoreApplication::applicationVersion() );
#ifdef WIN32
        url.addQueryItem( "utm_term", "WIN" );
#elif __APPLE__
        url.addQueryItem( "utm_term", "OSX" );
#elif defined (Q_WS_X11)
        url.addQueryItem( "utm_term", "X11" );
#endif
    }

    QDesktopServices::openUrl( url );
}
