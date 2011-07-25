#include "BannerWidget.h"

#include <QStackedLayout>
#include <QAbstractButton>
#include <QDesktopServices>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QStyle>
#include <QDebug>
#include <QCoreApplication>

class BannerWidgetPrivate : public QAbstractButton {
public:
    BannerWidgetPrivate( const QString& pText, QWidget* parent = 0 ):QAbstractButton(parent) {
        setText( QString( " " ) + pText + " " );
    }

protected:
    void paintEvent( QPaintEvent* /*e*/ ) {
        QPainter painter( this );

        painter.setRenderHint( QPainter::TextAntialiasing );
        painter.setRenderHint( QPainter::Antialiasing );

        QRect bgRect = m_textRect.adjusted( -20, 0, 20, 0 );

        painter.setWorldMatrix( m_transformMatrix );

        painter.fillRect( bgRect, palette().brush( QPalette::Window ));
        style()->drawItemText( &painter, m_textRect.translated( 0, -1 ), Qt::AlignHCenter, palette(), true, text() );
    }
    
    void resizeEvent( QResizeEvent* event )
    {
        clearMask();
        QFont f = font();
        m_textRect = QFontMetrics( f ).boundingRect( text() );   

        m_transformMatrix.reset();
    
        //Tiny optimization and means math.h doesn't need to be included
        //and saves some runtime ops. I shouldn't imagine sin(45) is likely to change anytime soon!
        const float sin45 = 0.707106781186548f;

        m_transformMatrix.translate( event->size().width() - ((sin45 * m_textRect.width()) + 6 ), (sin45 * m_textRect.height()) - 6 );
        m_transformMatrix.rotate( 45 );
        
        QRegion mask = m_transformMatrix.map( QRegion( m_textRect.adjusted( -20, 0, 20, 0 ) ) );
        setMask( mask );
    }

    void mousePressEvent( QMouseEvent* e )
    {
        if(! mask().contains( e->pos() )) {
            e->ignore();
            return;
        }

        e->accept();
        return QAbstractButton::mousePressEvent( e );
    }

    void mouseReleaseEvent( QMouseEvent* e ) {
        if(! mask().contains( e->pos() )) {
            e->ignore();
            return;
        }

        e->accept();

        return QAbstractButton::mouseReleaseEvent( e );
    }
    QMatrix m_transformMatrix;
    QRect m_textRect;
};

BannerWidget::BannerWidget( const QString& pText, QWidget* parent )
             :QWidget( parent ), m_childWidget( 0 )
{
    m_layout = new QStackedLayout( this );
    setLayout( m_layout );
    m_layout->setStackingMode( QStackedLayout::StackAll );
    m_layout->addWidget( m_banner = new BannerWidgetPrivate(pText) );
    connect( m_banner, SIGNAL( clicked() ), this, SLOT( onClick() ) );
    setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
}

void 
BannerWidget::setWidget( QWidget* w ) {
    //Remove any existing childWidget
    if( m_layout->count() > 1 ) {
        m_childWidget->removeEventFilter( this );
        m_layout->removeWidget( m_childWidget );
    }
    m_childWidget = w;
    m_childWidget->installEventFilter( this );
    m_layout->insertWidget( 0, m_childWidget );
    m_layout->setCurrentWidget( m_banner );
}

void 
BannerWidget::setHref( const QUrl& url )
{
    m_href = url;
}

void
BannerWidget::onClick()
{
    QDesktopServices::openUrl( m_href );
}

QSize 
BannerWidget::sizeHint()
{
    if( m_childWidget ) return m_childWidget->sizeHint();
    return QWidget::sizeHint();
}

void 
BannerWidget::mousePressEvent( QMouseEvent* e ) {
    QCoreApplication::sendEvent( m_layout, e );
}

bool
BannerWidget::eventFilter( QObject* o, QEvent* e )
{
    QWidget* w = qobject_cast<QWidget*>(o);
    if( !w ) return false;
    if( e->type() == QEvent::Resize ) {
        resize( static_cast<QResizeEvent*>(e)->size());
    }
    return false;
}

void 
BannerWidget::setBannerVisible( bool visible )
{
    m_banner->setVisible( visible );
}

bool 
BannerWidget::bannerVisible() const
{
    return m_banner->isVisible();
}
