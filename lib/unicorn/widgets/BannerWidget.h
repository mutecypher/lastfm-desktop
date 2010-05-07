#ifndef BANNER_WIDGET_H
#define BANNER_WIDGET_H

#include <QWidget>
#include <QAbstractButton>
#include <QMatrix>
#include <QRect>

#include "lib/DllExportMacro.h"

class UNICORN_DLLEXPORT BannerWidget : QAbstractButton {
Q_OBJECT
public:
    BannerWidget( const QString& text, QWidget* parent );

protected:
    void paintEvent( class QPaintEvent* e );
    void resizeEvent( class QResizeEvent* e );
    bool eventFilter( QObject* obj, QEvent* event );

    QMatrix matrix;
    QRect textRect;
};

#endif //BANNER_WIDGET_H

