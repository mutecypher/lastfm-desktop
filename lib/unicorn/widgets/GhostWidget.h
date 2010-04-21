#ifndef GHOST_WIDGET_H
#define GHOST_WIDGET_H

#include <QWidget>
#include "lib/unicorn/StylableWidget.h"
#include "lib/DllExportMacro.h"

class UNICORN_DLLEXPORT GhostWidget : public StylableWidget{
public:
    GhostWidget( QWidget* origin, QWidget* parent = 0 );

protected:
    bool eventFilter( QObject* obj, QEvent* event );
};

#endif //GHOST_WIDGET_H

