
#include <QDesktopServices>
#include <QUrl>

#include "DesktopServices.h"

unicorn::DesktopServices::DesktopServices()
{
}


bool
unicorn::DesktopServices::openUrl( QUrl url )
{
    if ( url.host() == "www.last.fm" )
        url.addQueryItem( "campaign", "client" );

    QDesktopServices::openUrl( url );
}
