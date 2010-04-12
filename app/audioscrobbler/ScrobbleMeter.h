#ifndef SCROBBLE_METER_H_
#define SCROBBLE_METER_H_
#include <QLabel>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QDebug>

class ScrobbleMeter : public QLabel {
Q_OBJECT
public:
    ScrobbleMeter( QWidget* p = 0 )
    :QLabel( p ),
     m_count( 0 )
    {
        setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    }
    
    /*QSize sizeHint() const
    {
         return QSize( width(), (float)width() / m_h4w );
    }*/


public slots:
    void setCount( const quint32 count )
    {
        m_count = count;
        setText( QString::number( m_count ));
    }
    
    void incrementCount()
    {
        m_count++;
        setText( QString::number( m_count ));
    }

protected:
/*    virtual void resizeEvent( QResizeEvent* e )
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
    }*/


    quint32 m_count;
    float m_h4w;
    int m_w;
};

#endif //SCROBBLE_METER_H_
