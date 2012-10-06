#ifndef BANNER_WIDGET_H
#define BANNER_WIDGET_H

#include <QFrame>
#include <QMatrix>
#include <QRect>
#include <QUrl>
#include <QStyle>
#include <QResizeEvent>
#include <QPainter>
#include <QAbstractButton>

#include "lib/DllExportMacro.h"
#include <QDebug>

class UNICORN_DLLEXPORT BannerWidget : public QFrame
{
    Q_OBJECT
public:
    BannerWidget( const QString& text, QWidget* parent = 0 );
    bool bannerVisible() const;
    void setWidget( QWidget* w );
    QSize sizeHint() const;

public slots:
    void setHref( const QUrl& url );
    void setBannerVisible( bool visible = true );

protected:
    void mousePressEvent( QMouseEvent* e );
    bool eventFilter( QObject*, QEvent* );

private slots:
    void onClick();

private:
    class QStackedLayout* m_layout;
    class BannerWidgetPrivate* m_banner;
    bool m_bannerVisible;

    QString m_href;
    QWidget* m_childWidget;
};



class BannerWidgetPrivate : public QAbstractButton
{
    Q_OBJECT
public:
    BannerWidgetPrivate( const QString& pText, QWidget* parent = 0 );

private:
    void paintEvent( QPaintEvent* /*e*/ );
    void resizeEvent( QResizeEvent* event );
    void mousePressEvent( QMouseEvent* e );
    void mouseReleaseEvent( QMouseEvent* e );

private:
    QMatrix m_transformMatrix;
    QRect m_textRect;
};

#endif //BANNER_WIDGET_H

