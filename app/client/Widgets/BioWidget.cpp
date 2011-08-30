
#include <QEventLoop>
#include <QApplication>
#include <QToolTip>

#include "lib/unicorn/widgets/BannerWidget.h"
#include "lib/unicorn/widgets/HttpImageWidget.h"

#include "WidgetTextObject.h"
#include "BioWidget.h"

BioWidget::BioWidget( QWidget* p ) 
          : QTextBrowser( p ), 
            m_currentHoverWidget(0)
{
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    connect(document()->documentLayout(), SIGNAL( documentSizeChanged(QSizeF)), SLOT( onBioChanged(QSizeF)));
    connect(this, SIGNAL(anchorClicked(QUrl)), SLOT(onAnchorClicked(QUrl)));

    m_widgetTextObject = new WidgetTextObject;
    viewport()->installEventFilter( this );
    document()->documentLayout()->registerHandler( WidgetImageFormat, m_widgetTextObject );

    ui.image = new HttpImageWidget(this);
    ui.image->setFixedSize( QSize( 150, 125 ) );
    ui.image->setAlignment( Qt::AlignTop );
    
    ui.onTour = new BannerWidget( tr("On Tour" ));
    ui.onTour->setBannerVisible( false );
    ui.onTour->setWidget( ui.image );

    ui.onTour->setFixedSize( QSize( 150, 125 ));
    insertWidget( ui.onTour );
    
    connect( ui.image, SIGNAL(loaded()), SLOT(update()));
    connect( this, SIGNAL(highlighted(QString)), SLOT(onHighlighted(QString)) );
}

void
BioWidget::onHighlighted( const QString& url )
{
    QToolTip::showText( cursor().pos(), url, this, QRect() );
}

void 
BioWidget::insertWidget( QWidget* w ) 
{
    QTextImageFormat widgetImageFormat;
    w->installEventFilter( this );
    widgetImageFormat.setObjectType( WidgetImageFormat );
    widgetImageFormat.setProperty( WidgetData, QVariant::fromValue<QWidget*>( w ) );
    
    QTextCursor cursor = textCursor();

    cursor.insertImage( widgetImageFormat, QTextFrameFormat::FloatLeft );
    setTextCursor( cursor );
}

bool 
BioWidget::eventFilter( QObject* o, QEvent* e ) {
    QWidget* w = qobject_cast<QWidget*>( o );
    if( viewport() == w ) {
        if( QEvent::MouseMove != e->type() ) return false;
        QMouseEvent* event = static_cast<QMouseEvent*>(e);
        //respect child widget cursor
        QWidget* w = m_widgetTextObject->widgetAtPoint(event->pos() );
        if( w != m_currentHoverWidget ) {
            m_currentHoverWidget = w;
            if( 0 == w ) {
                viewport()->unsetCursor();
            } else {
                QWidget* c = w->childAt(event->pos());
                c = c ? c : w;
                viewport()->setCursor( c->cursor());
            }
        }

        return false;
    }
    return false;
}


void
BioWidget::mousePressEvent( QMouseEvent* event )
{
    if(!sendMouseEvent(event)) {
        QTextBrowser::mousePressEvent( event );
    }
}


void
BioWidget::mouseReleaseEvent( QMouseEvent* event )
{
    if(!sendMouseEvent(event)) {
        QTextBrowser::mouseReleaseEvent( event );
    }
}


void
BioWidget::mouseMoveEvent( QMouseEvent* event ) 
{
    if(!sendMouseEvent(event)) {
        QTextBrowser::mouseMoveEvent( event );
    }
}

bool 
BioWidget::sendMouseEvent( QMouseEvent* event ) 
{
    QWidget* w = m_widgetTextObject->widgetAtPoint( event->pos());
    if( !w ) return false;

    QRectF wRect = m_widgetTextObject->widgetRect( w );
    QPoint pos = event->pos() - wRect.toRect().topLeft();

    QWidget* childWidget = w->childAt( event->pos());
    if( !childWidget ) {
        childWidget = w;
    } else {
        pos = childWidget->mapTo( w, pos );
    }

    QMouseEvent* widgetMouseEvent = new QMouseEvent( event->type(), pos, event->button(), event->buttons(), event->modifiers());

    QCoreApplication::postEvent( childWidget, widgetMouseEvent );
    event->accept();
    return true;
}

void 
BioWidget::onAnchorClicked( const QUrl& link )
{
    QDesktopServices::openUrl( link );
}

void 
BioWidget::onBioChanged( const QSizeF& size )
{
    setFixedHeight( size.toSize().height() );
}



void 
BioWidget::loadImage( const QUrl& url )
{
    ui.image->loadUrl( url );
}

void
BioWidget::setImageHref( const QUrl& href )
{
    ui.image->setHref( href );
}

void 
BioWidget::setOnTourVisible( bool visible, const QUrl& url )
{
    ui.onTour->setBannerVisible( visible );

    if( url.isValid()) {
        ui.onTour->setHref( url );
    }
    update();
}
