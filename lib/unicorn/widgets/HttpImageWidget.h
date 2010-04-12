#ifndef HTTP_IMAGE_WIDGET_H_
#define HTTP_IMAGE_WIDGET_H_

#include <QLabel>
#include <QUrl>
#include <QDesktopServices>
#include <lastfm.h>
#include <QPainter>
#include <QMouseEvent>
#include "lib/DllExportMacro.h"

class UNICORN_DLLEXPORT HttpImageWidget : public QLabel {
Q_OBJECT
public:
    HttpImageWidget( QWidget* parent = 0 )
    :QLabel( parent ), m_mouseDown( false ){}

public slots:
    void loadUrl( const QUrl& url, bool gradient = false )
    {
        m_gradient = gradient;
        clear();
        connect( lastfm::nam()->get(QNetworkRequest(url)), SIGNAL(finished()), SLOT(onUrlLoaded()));
    }

    void setHref( const QUrl& url )
    {
        m_href = url;
        if( m_href.isValid()) {
            setCursor( Qt::PointingHandCursor );
            connect( this, SIGNAL(clicked()), SLOT(onClick()));
        } else {
            setCursor( Qt::ArrowCursor );
            disconnect( this, SIGNAL( clicked()), this, SLOT(onClick()));
        }
    }

    
protected:
    virtual void mousePressEvent( QMouseEvent* event )
    {
        m_mouseDown = true;
    }

    virtual void mouseReleaseEvent( QMouseEvent* event )
    {
        if( m_mouseDown &&
            contentsRect().contains( event->pos() )) emit clicked();
        
        m_mouseDown = false;
    }

private slots:
    void onClick()
    {
        QDesktopServices::openUrl( m_href);
    }

    void onUrlLoaded()
    {
        QPixmap px;
        px.loadFromData(static_cast<QNetworkReply*>(sender())->readAll());
        
        if( m_gradient ) {
            QLinearGradient g(QPoint(), px.rect().bottomLeft());
            g.setColorAt( 0.0, QColor(0, 0, 0, 0.11*255));
            g.setColorAt( 1.0, QColor(0, 0, 0, 0.88*255));

            QPainter p(&px);
            p.setCompositionMode(QPainter::CompositionMode_Multiply);
            p.fillRect(px.rect(), g);
            p.end();
        }

        setFixedSize( px.size());
        setPixmap(px);
    }

signals:
    void clicked();

private:
    bool m_mouseDown;
    bool m_gradient;
    QUrl m_href;
};

#endif
