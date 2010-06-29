#include "GhostWidget.h"
#include <QResizeEvent>
#include <QDebug>

GhostWidget::GhostWidget( QWidget* origin, QWidget* parent )
            :StylableWidget( parent )
{
    setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    setFixedSize( origin->sizeHint() );
    origin->installEventFilter( this );
}

bool 
GhostWidget::eventFilter( QObject* obj, QEvent* event )
{
    if( event->type() == QEvent::Resize ) {
        QResizeEvent* re = static_cast<QResizeEvent*>( event );
        setFixedSize( re->size() );
    }
    return false;
}

