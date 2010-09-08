#include "BannerWidget.h"

#include <QDesktopServices>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QStyle>
#include <QDebug>

BannerWidget::BannerWidget( const QString& pText, QWidget* parent )
             :QAbstractButton( parent )
{
    setText( QString( " " ) + pText + " " );
    parent->installEventFilter( this );
    move( 0, 0 );
    connect( this, SIGNAL( clicked() ), this, SLOT( onClick() ) );
}

void 
BannerWidget::paintEvent( QPaintEvent* e )
{
    QPainter painter( this );

    painter.setRenderHint( QPainter::TextAntialiasing );
    painter.setRenderHint( QPainter::Antialiasing );

    //Tiny optimization and means math.h doesn't need to be included
    //and saves a runtime op. I shouldn't image sin(45) is likely to change anytime soon!

    QRect bgRect = m_textRect.adjusted( -20, 0, 20, 0 );

    painter.setWorldMatrix( m_transformMatrix );

    painter.fillRect( bgRect, palette().brush( QPalette::Window ));
    style()->drawItemText( &painter, m_textRect.translated( 0, -1 ), Qt::AlignHCenter, palette(), true, text() );
}

void 
BannerWidget::setHref( const QUrl& url )
{
    m_href = url;
}

void 
BannerWidget::resizeEvent( QResizeEvent* e )
{
    QAbstractButton::resizeEvent( e );
}

bool 
BannerWidget::eventFilter( QObject* obj, QEvent* event )
{
    Q_ASSERT( obj == parentWidget());
    if( event->type() == QEvent::Resize ) {
        clearMask();
        resize( parentWidget()->size());
     
        QFont f = font();
        m_textRect = QFontMetrics( f ).boundingRect( text() );   

        m_transformMatrix.reset();
        const float sin45 = 0.707106781186548f;
        m_transformMatrix.translate( rect().width() - ((sin45 * m_textRect.width()) + 6 ), (sin45 * m_textRect.height()) - 6 );
        m_transformMatrix.rotate( 45 );
        
        QRegion mask = m_transformMatrix.map( QRegion( m_textRect.adjusted( -20, 0, 20, 0 ) ) );
        setMask( mask );
    }
    return false;
}

void
BannerWidget::onClick()
{
    QDesktopServices::openUrl( m_href );
}

