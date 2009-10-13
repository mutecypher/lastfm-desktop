#include "RestWidget.h"
#include <QVBoxLayout>
#include <QLabel>

RestWidget::RestWidget( QWidget* p )
           :StylableWidget( p )
{
    new QVBoxLayout( this );
    layout()->addWidget( new QLabel( "Hi, blah" ));
}

