

#include "Drawer.h"


Drawer::Drawer( QWidget* parent )
    : QWidget( parent, Qt::Drawer | Qt::FramelessWindowHint )
    {
#ifdef FAKE_DRAWER
    timeLine = new QTimeLine( 1000, this );
    timeLine->setUpdateInterval( 1 );
    connect( timeLine, SIGNAL(frameChanged( int )), SLOT(onFrameChanged( int )));
    connect( timeLine, SIGNAL(finished()), SLOT(onTimeLineFinished()));
    parent->setMouseTracking( true );
    parent->installEventFilter( this );
    eventFilter( parent, 0 );
#endif
    }

#ifdef FAKE_DRAWER

bool
Drawer::eventFilter( QObject* obj, QEvent* event )
{
    QWidget* parent = static_cast<QWidget*>(obj);
    if( event == 0 ||   //called from constructor
        event->type() == QEvent::Move ||
        event->type() == QEvent::Resize ) {
        if( !isVisible() ) {
            resize( parent->size());
            move( parent->geometry().topLeft());
        } else {
            move( QPoint( parent->frameGeometry().right(), parent->geometry().y()));
            resize( width(), parent->size().height());
        }
        timeLine->setFrameRange( parentWidget()->frameGeometry().left(), parentWidget()->frameGeometry().right());
    }
    return false;
}

bool
Drawer::event( QEvent* event )
{
    switch( event->type() )
    {
        case QEvent::Show:
            parentWidget()->raise();
            timeLine->setDirection(QTimeLine::Forward);
            timeLine->setEasingCurve( QEasingCurve::OutExpo );
            timeLine->start();
        break;
        case QEvent::Close:
            parentWidget()->raise();
            timeLine->setDirection(QTimeLine::Backward);
            timeLine->setEasingCurve( QEasingCurve::InBounce );
            timeLine->start();
            event->setAccepted( false );
            return true;
        break;
    }

    return false;
}

void
Drawer::onFrameChanged( int frame )
{
    //move( frame, frameGeometry().y());
    SetWindowPos( winId(), parentWidget()->winId(),
                  frame, frameGeometry().y(),
                  frameGeometry().width(), frameGeometry().height(),
                  SWP_NOACTIVATE | SWP_NOOWNERZORDER );
}

void
Drawer::onTimeLineFinished()
{
    if( timeLine->direction() == QTimeLine::Backward)
        hide();
}

#endif // FAKE_DRAWER
