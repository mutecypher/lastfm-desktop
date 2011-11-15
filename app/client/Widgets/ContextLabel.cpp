
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
    static QPixmap arrow( ":/meta_context_arrow.png" );

    qWarning() << arrow.size();

    QPainter p;
    p.begin( this );

    QPoint arrowPoint = QPoint( ( 128 + 10 - arrow.size().width() )  / 2, 21 - arrow.size().height() );

    p.drawPixmap( arrowPoint, arrow );

    p.end();
}
