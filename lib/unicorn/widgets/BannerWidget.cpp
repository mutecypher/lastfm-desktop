#include "BannerWidget.h"

#include <QStackedLayout>
#include <QDesktopServices>
#include <QMoveEvent>
#include <QDebug>
#include <QCoreApplication>

BannerWidget::BannerWidget( const QString& pText, QWidget* parent )
             :QFrame( parent ), m_childWidget( 0 )
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
    if( url.isValid()) {
        setCursor( Qt::PointingHandCursor );
    } else {
        unsetCursor();
    }
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
