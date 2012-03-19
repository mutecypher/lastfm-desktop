
#include <QPainter>
#include <QDebug>

#include "ContextLabel.h"

ContextLabel::ContextLabel( QWidget* parent )
    :unicorn::Label( parent )
{
}

void
ContextLabel::paintEvent( QPaintEvent* event )
{
    QLabel::paintEvent( event );

    // draw the arrow on the context
    QPainter p;
    p.begin( this );

    static QPixmap arrow( ":/meta_context_arrow.png" );

    // these values match the ones in the stylesheet
    int topMargin = 20;
    int leftMargin = 20;

    if ( this->objectName() == "userBlurb" )
    {
        topMargin = 12;
        leftMargin = 0;
    }

    QPoint arrowPoint = QPoint( leftMargin + (( 126 + 10 - arrow.size().width() )  / 2 ), topMargin + 1 - arrow.size().height() );
    p.drawPixmap( arrowPoint, arrow );

    p.end();
}
