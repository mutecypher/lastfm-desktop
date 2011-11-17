#ifndef BIO_WIDGET_H_
#define BIO_WIDGET_H_

#include <QTextBrowser>
#include <QTextObjectInterface>
#include <QDebug>
#include "lib/unicorn/widgets/HttpImageWidget.h"

/** A specialized QTextBrowser which can insert widgets inline */
#include <QPlainTextDocumentLayout>
class BioWidget : public QTextBrowser
{
    Q_OBJECT
public:
    BioWidget( QWidget* parent );
    bool eventFilter( QObject* o, QEvent* e );
    
    void setBioText( const QString& bioText );

    void setPixmap( const QPixmap& pixmap );
    void loadImage( const QUrl&, HttpImageWidget::ScaleType scale = HttpImageWidget::ScaleAuto );
    void setImageHref( const QUrl& );
    
    void setOnTourVisible( bool, const QUrl& = QUrl());

protected slots:
    void onBioChanged( const QSizeF& size );
    void onAnchorClicked( const QUrl& link );

    void onHighlighted( const QString& url );

    void onImageLoaded();

    void appendBioText();
    void updateGeometryPlease();
    void onDocumentLayoutChanged();

protected:
    void insertWidget( QWidget* w );
    class WidgetTextObject* m_widgetTextObject;

    void mousePressEvent( QMouseEvent* event );
    void mouseReleaseEvent( QMouseEvent* event );
    void mouseMoveEvent( QMouseEvent* event );

    bool sendMouseEvent( QMouseEvent* event );

    enum WidgetProperties { WidgetData = 1 };
    enum { WidgetImageFormat = QTextFormat::UserObject + 1 };
    QWidget* m_currentHoverWidget;

    QString m_bioText;

    QTextImageFormat m_widgetImageFormat;

    struct {
        class BannerWidget* onTour;
        class HttpImageWidget* image;
    } ui;
};


#endif //BIO_WIDGET_H_
