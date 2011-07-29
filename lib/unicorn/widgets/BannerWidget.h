#ifndef BANNER_WIDGET_H
#define BANNER_WIDGET_H

#include <QFrame>
#include <QMatrix>
#include <QRect>
#include <QUrl>

#include "lib/DllExportMacro.h"
#include <QDebug>

class UNICORN_DLLEXPORT BannerWidget : public QFrame {
Q_OBJECT
public:
    BannerWidget( const QString& text, QWidget* parent = 0 );
    bool bannerVisible() const;
    void setWidget( QWidget* w );
    virtual QSize sizeHint();

public slots:
    void setHref( const QUrl& url );
    void setBannerVisible( bool = true );

protected:
    void mousePressEvent( QMouseEvent* e );
    bool eventFilter( QObject*, QEvent* );
    bool m_bannerVisible;

private slots:
    void onClick();

private:
    class QStackedLayout* m_layout;
    class BannerWidgetPrivate* m_banner;
    QUrl m_href;
    QWidget* m_childWidget;
};


#include <QStyle>
#include <QResizeEvent>
#include <QPainter>
#include <QAbstractButton>
class BannerWidgetPrivate : public QAbstractButton {
    Q_OBJECT
public:
    BannerWidgetPrivate( const QString& pText, QWidget* parent = 0 ):QAbstractButton(parent) {
        setText( QString( " " ) + pText + " " );
    }

protected:
    void paintEvent( QPaintEvent* /*e*/ ) {
        QPainter painter( this );

        painter.setRenderHint( QPainter::TextAntialiasing );
        painter.setRenderHint( QPainter::Antialiasing );

        QRect bgRect = m_textRect.adjusted( -20, 0, 20, 0 );

        painter.setWorldMatrix( m_transformMatrix );

        painter.fillRect( bgRect, palette().brush( QPalette::Window ));
        style()->drawItemText( &painter, m_textRect.translated( 0, -1 ), Qt::AlignCenter, palette(), true, text() );
    }
    
    void resizeEvent( QResizeEvent* event )
    {
        clearMask();
        QFont f = font();
        m_textRect = QFontMetrics( f ).boundingRect( text() );   
        m_textRect.adjust( 0, 0, 0, 5 );
        m_transformMatrix.reset();
    
        //Tiny optimization and means math.h doesn't need to be included
        //and saves some runtime ops. I shouldn't imagine sin(45) is likely to change anytime soon!
        const float sin45 = 0.707106781186548f;

        m_transformMatrix.translate( event->size().width() - ((sin45 * m_textRect.width()) + 6 ), (sin45 * m_textRect.height()) - 6 );
        m_transformMatrix.rotate( 45 );
        
        QRegion mask = m_transformMatrix.map( QRegion( m_textRect.adjusted( -20, 0, 20, 0 ) ) );
        setMask( mask );
    }

    void mousePressEvent( QMouseEvent* e )
    {
        if(! mask().contains( e->pos() )) {
            e->ignore();
            return;
        }

        e->accept();
        return QAbstractButton::mousePressEvent( e );
    }

    void mouseReleaseEvent( QMouseEvent* e ) {
        if(! mask().contains( e->pos() )) {
            e->ignore();
            return;
        }

        e->accept();

        return QAbstractButton::mouseReleaseEvent( e );
    }
    QMatrix m_transformMatrix;
    QRect m_textRect;
};

#endif //BANNER_WIDGET_H

