#ifndef DRAWER_H_
#define DRAWER_H_

#include <QWidget>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QTimeLine>
#include <QApplication>
#include <windows.h>

#ifdef Q_OS_WIN
    #define FAKE_DRAWER
#endif
class Drawer : public QWidget {
Q_OBJECT
public:
    Drawer( QWidget* parent ) : QWidget( parent, Qt::Drawer | 
                                                 Qt::FramelessWindowHint ) 
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

protected:
#ifdef FAKE_DRAWER
    bool eventFilter( QObject* obj, QEvent* event ) {
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

    bool event( QEvent* event ) {
        switch( event->type() ) {
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
#endif

private slots:
#ifdef FAKE_DRAWER
    void onFrameChanged( int frame ) {
        //move( frame, frameGeometry().y());
        SetWindowPos( winId(), parentWidget()->winId(), 
                      frame, frameGeometry().y(), 
                      frameGeometry().width(), frameGeometry().height(), 
                      SWP_NOACTIVATE | SWP_NOOWNERZORDER );
    }
    void onTimeLineFinished() {
        if( timeLine->direction() == QTimeLine::Backward)
            hide();
    }
#endif
private:
#ifdef FAKE_DRAWER
    QTimeLine* timeLine;
#endif
};

#endif //DRAWER_H_
