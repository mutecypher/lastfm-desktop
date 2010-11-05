#ifndef POINTY_ARROW_H
#define POINTY_ARROW_H

#include <QWidget>

class PointyArrow : public QWidget {
    Q_OBJECT

public:
    PointyArrow();
    ~PointyArrow();

    void pointAt( const QPoint& );

protected:
    virtual void paintEvent( QPaintEvent* );
    class QPixmap* m_pm;
    class QTimeLine* m_timeline;

protected slots:
    void onFrameChanged( int );
    void onFinished();
};

#endif //POINTY_ARROW_H

