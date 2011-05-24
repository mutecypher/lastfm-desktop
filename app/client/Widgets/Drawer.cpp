

#include "Drawer.h"


Drawer::Drawer( QWidget* parent )
    : QWidget( parent, Qt::Drawer | Qt::FramelessWindowHint )
    {
#ifdef FAKE_DRAWER
    m_timeLine = new QTimeLine( 1000, this );
    m_timeLine->setUpdateInterval( 1 );
    connect( m_timeLine, SIGNAL(frameChanged( int )), SLOT(onFrameChanged( int )));
    connect( m_timeLine, SIGNAL(finished()), SLOT(onTimeLineFinished()));
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
    if( event == 0 ||   // called from constructor
        event->type() == QEvent::Move ||
        event->type() == QEvent::Resize ) {
        if( !isVisible() ) {
            resize( parent->size());
            move( parent->geometry().topLeft());
        } else {
            move( QPoint( parent->frameGeometry().right(), parent->geometry().y()));
            resize( width(), parent->size().height());
        }
        m_timeLine->setFrameRange( parentWidget()->frameGeometry().left(), parentWidget()->frameGeometry().right());
    }

    return false;
}

void
Drawer::showEvent( QShowEvent* event )
{
    parentWidget()->raise();
    m_timeLine->setDirection(QTimeLine::Forward);
    m_timeLine->setEasingCurve( QEasingCurve::OutExpo );
    m_timeLine->start();
}

void
Drawer::closeEvent( QCloseEvent* event )
{
    parentWidget()->raise();
    m_timeLine->setDirection(QTimeLine::Backward);
    m_timeLine->setEasingCurve( QEasingCurve::InBounce );
    m_timeLine->start();
    event->setAccepted( false );
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
    if( m_timeLine->direction() == QTimeLine::Backward)
        hide();
}

#endif // FAKE_DRAWER
