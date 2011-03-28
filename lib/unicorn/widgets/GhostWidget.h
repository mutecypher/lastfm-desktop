#ifndef GHOST_WIDGET_H
#define GHOST_WIDGET_H

#include <QWidget>
#include "lib/DllExportMacro.h"

class UNICORN_DLLEXPORT GhostWidget : public QWidget
{
public:
    GhostWidget(QWidget* parent = 0 );
    void setOrigin( QWidget* origin );

protected:
    bool eventFilter( QObject* obj, QEvent* event );
};

#endif //GHOST_WIDGET_H

