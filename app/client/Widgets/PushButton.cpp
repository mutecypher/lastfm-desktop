
#include <QPixmap>
#include <QPainter>
#include <QEvent>

#include "PushButton.h"

PushButton::PushButton(QWidget *parent) :
    QPushButton(parent),
    m_hovered( false )
{
    setAttribute( Qt::WA_LayoutUsesWidgetRect );
    setAttribute( Qt::WA_Hover );
    setAttribute( Qt::WA_MacNoClickThrough );

    setCursor( Qt::PointingHandCursor );
}

bool
PushButton::event( QEvent* e )
{
    switch ( e->type() )
    {
    case QEvent::HoverEnter:
        m_hovered = true;
        update();
        break;
    case QEvent::HoverLeave:
        m_hovered = false;
        update();
        break;
    default:
        break;
    }

    return QPushButton::event( e );
}

void
PushButton::paintEvent(QPaintEvent *)
{
    static QPixmap leftHover( ":/button_LEFT_HOVER.png" );
    static QPixmap leftPress( ":/button_LEFT_PRESS.png" );
    static QPixmap leftRest( ":/button_LEFT_REST.png" );
    static QPixmap middleHover( ":/button_MIDDLE_HOVER.png" );
    static QPixmap middlePress( ":/button_MIDDLE_PRESS.png" );
    static QPixmap middleRest( ":/button_MIDDLE_REST.png" );
    static QPixmap rightHover( ":/button_RIGHT_HOVER.png" );
    static QPixmap rightPress( ":/button_RIGHT_PRESS.png" );
    static QPixmap rightRest( ":/button_RIGHT_REST.png" );

    QPainter p( this );

    QRect middleRect = QRect( leftRest.width(), 0, rect().width() - leftRest.width() - rightRest.width(), 49 );

    if ( isDown() )
    {
        p.drawPixmap( rect().topLeft(), leftPress );
        p.drawPixmap( middleRect, middlePress );
        p.drawPixmap( rect().topLeft() + QPoint( rect().width() - rightPress.width(), 0 ), rightPress );
    }
    else if ( m_hovered )
    {
        p.drawPixmap( rect().topLeft(), leftHover );
        p.drawPixmap( middleRect, middleHover );
        p.drawPixmap( rect().topLeft() + QPoint( rect().width() - rightHover.width(), 0 ), rightHover );
    }
    else
    {
        p.drawPixmap( rect().topLeft(), leftRest );
        p.drawPixmap( middleRect, middleRest );
        p.drawPixmap( rect().topLeft() + QPoint( rect().width() - rightRest.width(), 0 ), rightRest );
    }

    QFontMetrics fmText( font() );
    QRect textRect = contentsRect();
    p.drawText( textRect, Qt::AlignCenter, fmText.elidedText( text(), Qt::ElideRight, textRect.width() ) );
}
