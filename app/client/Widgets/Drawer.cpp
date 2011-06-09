

#include "Drawer.h"
#include <QDebug>
#include <QMainWindow>


Drawer::Drawer( QWidget* parent )
       : QDockWidget( parent, Qt::Drawer | Qt::FramelessWindowHint )
{
    setMinimumWidth( 450 );
    hide();
#ifdef FAKE_DRAWER
    connect( this, SIGNAL( dockLocationChanged(Qt::DockWidgetArea)),
                   SLOT( onDockLocationChanged(Qt::DockWidgetArea)));
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
void 
Drawer::onDockLocationChanged( Qt::DockWidgetArea area ) {
    m_dockWidgetArea = area;
    if( area == Qt::RightDockWidgetArea ) {
        m_timeLine->setFrameRange( parentWidget()->frameGeometry().left(), parentWidget()->frameGeometry().right());
    } else {
        m_timeLine->setFrameRange( parentWidget()->frameGeometry().left(), parentWidget()->frameGeometry().left() - width());
    }
}

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
            if( m_dockWidgetArea == Qt::RightDockWidgetArea )
                move( QPoint( parent->frameGeometry().right(), parent->geometry().y()));
            else
                move( QPoint( parent->frameGeometry().left() - width(), parent->geometry().y()));
            resize( width(), parent->size().height());

        }
//        m_timeLine->setFrameRange( parentWidget()->frameGeometry().left(), parentWidget()->frameGeometry().right());
    }

    return false;
}

void
Drawer::showEvent( QShowEvent* event )
{
    qDebug() << "Allowed Areas" << allowedAreas();
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
