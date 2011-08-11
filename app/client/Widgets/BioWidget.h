#ifndef BIO_WIDGET_H_
#define BIO_WIDGET_H_

#include <QTextBrowser>
#include <QTextObjectInterface>
#include <QDebug>

/** An embeddable widget text object wrapper */
class WidgetTextObject : public QObject, QTextObjectInterface {
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)
    const QSize kMargin;
public:
    WidgetTextObject():kMargin(20,10){};
    virtual QSizeF intrinsicSize(QTextDocument*, int /*posInDocument*/,
                                 const QTextFormat& format) {
        QWidget* widget = qVariantValue<QWidget*>(format.property(1));
        return QSizeF( widget->size() + kMargin );
    }

    virtual void drawObject(QPainter *painter, const QRectF &rect,
           QTextDocument * /*doc*/, int /*posInDocument*/,
           const QTextFormat &format) {
        QWidget* widget = qVariantValue<QWidget*>(format.property( 1 ));
        widget->render( painter, QPoint( 0, 0 ));
        
        //Adjusted to allow for the margin
        QRect contentsRect = rect.toRect().adjusted(0, 0, -kMargin.width(), -kMargin.height());
        m_widgetRects[widget] = contentsRect;
    }

    QWidget* widgetAtPoint( const QPoint& p ) {
        QMapIterator<QWidget*, QRect> i(m_widgetRects);
        while (i.hasNext()) {
            i.next();
            if( i.value().contains(p))
                return i.key();
        }
        return 0;
    }

    QRectF widgetRect( QWidget* w ) {
        return m_widgetRects[w];
    }

protected:
    QMap<QWidget*, QRect> m_widgetRects;
};

/** A specialized QTextBrowser which can insert widgets inline */
#include <QPlainTextDocumentLayout>
class BioWidget : public QTextBrowser {
    Q_OBJECT
public:
    BioWidget( QWidget* parent );
    bool eventFilter( QObject* o, QEvent* e );
    
    void loadImage( const QUrl& );
    void setImageHref( const QUrl& );
    
    void setOnTourVisible( bool, const QUrl& = QUrl());

protected slots:
    void onBioChanged( const QSizeF& size );
    void onAnchorClicked( const QUrl& link );

protected:
    void insertWidget( QWidget* w );
    WidgetTextObject* m_widgetTextObject;

    void mousePressEvent( QMouseEvent* event );
    void mouseReleaseEvent( QMouseEvent* event );
    void mouseMoveEvent( QMouseEvent* event );

    bool sendMouseEvent( QMouseEvent* event );

    enum WidgetProperties { WidgetData = 1 };
    enum { WidgetImageFormat = QTextFormat::UserObject + 1 };
    QWidget* m_currentHoverWidget;

    struct {
        class BannerWidget* onTour;
        class HttpImageWidget* image;
    } ui;
};


#endif //BIO_WIDGET_H_
