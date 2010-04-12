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
     m_count( 0 ) { } 

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
    quint32 m_count;
};

#endif //SCROBBLE_METER_H_
