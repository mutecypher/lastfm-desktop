#ifndef BANNER_WIDGET_H
#define BANNER_WIDGET_H

#include <QWidget>
#include <QAbstractButton>
#include <QMatrix>
#include <QRect>
#include <QUrl>

#include "lib/DllExportMacro.h"

class UNICORN_DLLEXPORT BannerWidget : public QAbstractButton {
Q_OBJECT
public:
    BannerWidget( const QString& text, QWidget* parent );

public slots:
    void setHref( const QUrl& url );

protected:
    void mousePressEvent( QMouseEvent * e );
    void mouseReleaseEvent( QMouseEvent * e );
    void paintEvent( class QPaintEvent* e );
    void resizeEvent( class QResizeEvent* e );
    bool eventFilter( QObject* obj, QEvent* event );

    QMatrix m_transformMatrix;
    QRect m_textRect;

private slots:
    void onClick();

private:
    QUrl m_href;
};

#endif //BANNER_WIDGET_H

