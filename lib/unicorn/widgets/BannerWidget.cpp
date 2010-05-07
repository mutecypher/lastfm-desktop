#include "BannerWidget.h"

#include <QMoveEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QStyle>
#include <QDebug>

BannerWidget::BannerWidget( const QString& text, QWidget* parent )
             :QAbstractButton( parent )
{
    setText( QString( " " ) + text + " " );
    parent->installEventFilter( this );
    move( 0, 0 );
    setCursor( QCursor( Qt::ArrowCursor ) );
}

void 
BannerWidget::paintEvent( QPaintEvent* e )
{
    QPainter painter( this );

    painter.setRenderHint( QPainter::TextAntialiasing );
    painter.setRenderHint( QPainter::Antialiasing );

    QFont f = font();
    QRect textRect  = QFontMetrics( f ).boundingRect( text() );

    QMatrix matrix;
    
    //Tiny optimization and means math.h doesn't need to be included
    //and saves a runtime op. I shouldn't image sin(45) is likely to change anytime soon!
    const float sin45 = 0.707106781186548f;

    matrix.translate( width() - ((sin45 * textRect.width() + 6) ), (sin45 * textRect.height()) - 6 );
    matrix.rotate( 45 );
    
    QRect bgRect = textRect.adjusted( -20, 0, 20, 0 );
    QRegion mask = matrix.map( QRegion( bgRect ) );
    setMask( mask );

    painter.setWorldMatrix( matrix );
    
    painter.fillRect( bgRect, palette().brush( QPalette::Window ));
    style()->drawItemText( &painter, textRect.translated( 0, -1 ), Qt::AlignHCenter, palette(), true, text() );
}

void 
BannerWidget::resizeEvent( QResizeEvent* )
{

}

bool 
BannerWidget::eventFilter( QObject* obj, QEvent* event )
{
    Q_ASSERT( obj == parentWidget());
    if( event->type() == QEvent::Resize ) {
        resize( static_cast<QResizeEvent*>( event )->size());
    }
    return false;
}
