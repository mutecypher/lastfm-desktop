
#include <QWidget>

#include "WidgetTextObject.h"

WidgetTextObject::WidgetTextObject()
    :kMargin(20,10)
{
}

QSizeF
WidgetTextObject::intrinsicSize(QTextDocument*, int /*posInDocument*/, const QTextFormat& format)
{
    QWidget* widget = qVariantValue<QWidget*>(format.property(1));
    return QSizeF( widget->size() + kMargin );
}

void
WidgetTextObject::drawObject(QPainter *painter, const QRectF &rect, QTextDocument * /*doc*/, int /*posInDocument*/, const QTextFormat &format)
{
    QWidget* widget = qVariantValue<QWidget*>(format.property( 1 ));
    widget->render( painter, QPoint( 0, 0 ));

    //Adjusted to allow for the margin
    QRect contentsRect = rect.toRect().adjusted(0, 0, -kMargin.width(), -kMargin.height());
    m_widgetRects[widget] = contentsRect;
}

QWidget*
WidgetTextObject::widgetAtPoint( const QPoint& p )
{
    QMapIterator<QWidget*, QRect> i(m_widgetRects);
    while (i.hasNext())
    {
        i.next();
        if( i.value().contains(p))
            return i.key();
    }
    return 0;
}

QRectF
WidgetTextObject::widgetRect( QWidget* w )
{
    return m_widgetRects[w];
}
