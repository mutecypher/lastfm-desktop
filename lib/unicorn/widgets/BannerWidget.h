#ifndef BANNER_WIDGET_H
#define BANNER_WIDGET_H

#include <QWidget>
#include <QMatrix>
#include <QRect>
#include <QUrl>

#include "lib/DllExportMacro.h"
#include <QDebug>

class UNICORN_DLLEXPORT BannerWidget : public QWidget {
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

#endif //BANNER_WIDGET_H

