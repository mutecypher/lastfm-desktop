#include "AnimatedStatusBar.h"

#include <QTimeLine>
#include "qtwin.h"
#include <qDebug>

AnimatedStatusBar::AnimatedStatusBar( QWidget* parent )
                  :QStatusBar( parent )
{
    m_timeline = new QTimeLine( 50, this );
    m_timeline->setUpdateInterval( 20 );
    m_timeline->setCurveShape( QTimeLine::EaseInCurve );
    connect( m_timeline, SIGNAL( frameChanged(int)), SLOT(onFrameChanged(int)));
    connect( m_timeline, SIGNAL( finished()), SLOT(onFinished()));
}


void
AnimatedStatusBar::showAnimated()
{
    if( isVisible() && height() > 0 ) return;
    
    window()->setMinimumHeight( window()->height());
    m_timeline->setFrameRange( 0, sizeHint().height());
    m_timeline->setDirection( QTimeLine::Forward );
    setFixedHeight( 0 );
    show();
    m_windowHeight = window()->height();
    m_timeline->start();
}


void 
AnimatedStatusBar::hideAnimated()
{
    if( !isVisible() || height() == 0 ) return;
    
    window()->setMaximumHeight( window()->height());
    m_timeline->setFrameRange( 0, sizeHint().height());
    m_timeline->setDirection( QTimeLine::Backward );
    
    m_windowHeight = window()->height();
    m_timeline->start();
}

void 
AnimatedStatusBar::onFinished()
{
    if( m_timeline->direction() == QTimeLine::Backward ) {
        hide();
    }

    setMaximumHeight( QWIDGETSIZE_MAX );
    setMinimumHeight( 0 );
    window()->setMinimumHeight( 0 );
    window()->setMaximumHeight( QWIDGETSIZE_MAX );
}


void 
AnimatedStatusBar::onFrameChanged( int f )
{
    setFixedHeight( f );
    if( m_timeline->direction() == QTimeLine::Forward )
        window()->resize( window()->width(), m_windowHeight + f );
    else
        window()->resize( window()->width(), m_windowHeight - (sizeHint().height() - f ) );
}
