#ifndef DESKTOPSERVICES_H
#define DESKTOPSERVICES_H

#include <QUrl>

#include "lib/DllExportMacro.h"

namespace unicorn
{

class UNICORN_DLLEXPORT DesktopServices
{
public:
    DesktopServices();

    static void openUrl( QUrl url );
};

}

#endif // DESKTOPSERVICES_H
