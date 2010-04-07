#ifndef SCROBBLE_METER_H_
#define SCROBBLE_METER_H_
#include "lib/unicorn/StylableWidget.h"
#include <QPaintEvent>
#include <QResizeEvent>
#include <QStylePainter>
#include <QDebug>
#include <QStyleOption>

class ScrobbleMeter : public StylableWidget {
Q_OBJECT
public:
    ScrobbleMeter( QWidget* p = 0 )
    :StylableWidget( p )
    {
        setAutoFillBackground( false );
        setFrameShape( QFrame::Box );
        setFrameShadow( QFrame::Plain );
        setLineWidth( 1 );

        setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );

        style()->polish( this );
    }

    QSize sizeHint() const
    {
         return QSize( width(), (float)width() / m_h4w );
    }


public slots:
    void setCount( const quint32 count )
    {
        m_count = count;
        update();
    }

protected:
    virtual void resizeEvent( QResizeEvent* e )
    {
        QString s = QString::number( m_count );
        float factor = ((float)e->size().width() - ( 4 * (s.count()+2)) ) / fontMetrics().width( s );
        QFont f = font();
        f.setPointSizeF( f.pointSizeF() * factor );
        if( QFontMetrics( f ).height() > e->size().height() ) {
            factor = (float)e->size().height() / QFontMetrics( f ).height();
            f.setPointSizeF( f.pointSizeF() * factor );
            setFont( f );
        }
        setFont( f );
        m_h4w = (float)QFontMetrics( f ).width( s ) / QFontMetrics( f ).height();
        m_w = QFontMetrics( f ).width( s );
        updateGeometry();
    }


    void paintEvent( QPaintEvent* event )
    {
        QStylePainter p( this );

        QString s = QString::number( m_count );
       
        QRect r = rect();
        r.setLeft( r.left() + 2 );
        QStyleOptionFrame opt;
        opt.initFrom( this );

        foreach( QChar c, s ) {
            QRectF br;
            p.drawText( r, Qt::AlignLeft, c, &br );
            br.setRight( br.right() + 2 );
            //opt.rect = br.toRect();
            //style()->drawPrimitive( QStyle::PE_Frame, &opt, &p, this );
            //p.drawRect( br );
            r.setLeft( br.right() + 2 );
        }

    }

    quint32 m_count;
    float m_h4w;
    int m_w;
};

#endif //SCROBBLE_METER_H_
