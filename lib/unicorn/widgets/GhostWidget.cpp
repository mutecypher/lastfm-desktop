#include "GhostWidget.h"
#include <QResizeEvent>
#include <QDebug>

GhostWidget::GhostWidget( QWidget* parent )
    :QWidget( parent )
{
}

void
GhostWidget::setOrigin( QWidget* origin )
{
    setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    setFixedSize( origin->sizeHint() );
    origin->installEventFilter( this );
}

bool 
GhostWidget::eventFilter( QObject* /*obj*/, QEvent* event )
{
    if( event->type() == QEvent::Resize ) {
        QResizeEvent* re = static_cast<QResizeEvent*>( event );
        setFixedSize( re->size() );
    }
    return false;
}

