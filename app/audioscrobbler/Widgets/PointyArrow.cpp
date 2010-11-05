#include "PointyArrow.h"
#include <QDebug>
#include <QPixmap>
#include <QPainter>
#include <QTimeLine>

PointyArrow::PointyArrow()
{
    m_pm = new QPixmap( ":/pointyarrow.png" );

    resize( m_pm->size());
    setAttribute( Qt::WA_TranslucentBackground );
    setWindowFlags( Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint );
    
    move( geometry().left(), 40 );
    m_timeline = new QTimeLine( 1000 );


    connect( m_timeline, SIGNAL( frameChanged( int )), SLOT( onFrameChanged( int )));
    connect( m_timeline, SIGNAL( finished()), SLOT( onFinished()));
}

PointyArrow::~PointyArrow()
{
    delete m_pm;
    delete m_timeline;
}

void 
PointyArrow::paintEvent( QPaintEvent* event )
{
    QPainter p( this );
    p.drawPixmap( QPoint( 0, 0 ), *m_pm );
}

void 
PointyArrow::onFrameChanged( int frame )
{
    move( geometry().left(), frame );
}

void 
PointyArrow::onFinished()
{
    m_timeline->toggleDirection();
    m_timeline->start();
}

void
PointyArrow::pointAt( const QPoint& point )
{ 
    m_timeline->stop();
    move( point.x() - (width() / 2.0f), point.y() + 40 );
    m_timeline->setCurveShape( QTimeLine::EaseInOutCurve );
    m_timeline->setFrameRange( geometry().top(), geometry().top() + 40 );
    m_timeline->start();
    show();
}
