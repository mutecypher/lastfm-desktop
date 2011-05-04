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
    setVisible( origin->isVisible() );
    origin->installEventFilter( this );
}

bool 
GhostWidget::eventFilter( QObject* /*obj*/, QEvent* event )
{
    switch ( event->type() )
    {
    case QEvent::Resize:
        {
        QResizeEvent* re = static_cast<QResizeEvent*>( event );
        setFixedSize( re->size() );
        }
        break;
    case QEvent::Show:
        show();
        break;
    case QEvent::Hide:
        hide();
        break;
    }

    return false;
}

