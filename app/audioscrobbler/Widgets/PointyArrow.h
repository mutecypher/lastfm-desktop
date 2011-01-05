#ifndef POINTY_ARROW_H
#define POINTY_ARROW_H

#include <QWidget>
#include <QPixmap>

class PointyArrow : public QWidget {
    Q_OBJECT

public:
    PointyArrow();
    ~PointyArrow();

    void pointAt( const QPoint& );

protected:
    virtual void paintEvent( QPaintEvent* );
    QPixmap m_pm;
    class QTimeLine* m_timeline;

	enum { DirectionUp, DirectionDown, DirectionLeft, DirectionRight } m_currentDirection;

protected slots:
    void onFrameChanged( int );
    void onFinished();
};

#endif //POINTY_ARROW_H

