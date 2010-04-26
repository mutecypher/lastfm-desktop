#ifndef GHOST_WIDGET_H
#define GHOST_WIDGET_H

#include <QWidget>
#include "lib/unicorn/StylableWidget.h"

class GhostWidget : public StylableWidget 
{
    Q_OBJECT
public:
    GhostWidget( QWidget* origin, QWidget* parent = 0 );

protected:
    bool eventFilter( QObject* obj, QEvent* event );
};

#endif //GHOST_WIDGET_H

