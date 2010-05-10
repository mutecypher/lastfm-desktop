#ifndef BANNER_WIDGET_H
#define BANNER_WIDGET_H

#include <QWidget>
#include <QAbstractButton>
#include <QMatrix>
#include <QRect>

class BannerWidget : QAbstractButton {
Q_OBJECT
public:
    BannerWidget( const QString& text, QWidget* parent );

protected:
    void paintEvent( class QPaintEvent* e );
    void resizeEvent( class QResizeEvent* e );
    bool eventFilter( QObject* obj, QEvent* event );

    QMatrix m_transformMatrix;
    QRect m_textRect;
};

#endif //BANNER_WIDGET_H

