#ifndef WIDGETTEXTOBJECT_H
#define WIDGETTEXTOBJECT_H

#include <QObject>
#include <QTextObjectInterface>

/** An embeddable widget text object wrapper */
class WidgetTextObject : public QObject, QTextObjectInterface {
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)
    const QSize kMargin;

public:
    WidgetTextObject();
    QSizeF intrinsicSize(QTextDocument*, int /*posInDocument*/, const QTextFormat& format);
    void drawObject(QPainter *painter, const QRectF &rect, QTextDocument * /*doc*/, int /*posInDocument*/, const QTextFormat &format);

    QWidget* widgetAtPoint( const QPoint& p );

    QRectF widgetRect( QWidget* w );

protected:
    QMap<QWidget*, QRect> m_widgetRects;
};

#endif // WIDGETTEXTOBJECT_H
