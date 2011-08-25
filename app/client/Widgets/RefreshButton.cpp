
#include <QPainter>

#include "RefreshButton.h"

RefreshButton::RefreshButton(QWidget *parent) :
    QPushButton(parent), m_pixmap( ":/scrobbles_refresh.png" )
{
}

void
RefreshButton::paintEvent( QPaintEvent* e )
{
    QPushButton::paintEvent( e );

    QPainter p( this );
    QFontMetrics fm( font() );
    p.drawPixmap( rect().center() - QPoint( ( fm.width( text() ) / 2 ) + m_pixmap.width() + 10, ( m_pixmap.height() / 2 ) - 1 ), m_pixmap );
}
