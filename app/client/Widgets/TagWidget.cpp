
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDesktopServices>
#include <QPainter>
#include <QFontMetrics>

#include "lib/unicorn/widgets/Label.h"

#include "TagWidget.h"

TagWidget::TagWidget( const QString& tag, const QString& url, QWidget *parent )
    :QPushButton( tag, parent ),
      m_url( url )
{    
    connect( this, SIGNAL(clicked()), SLOT(onClicked()));

    setAttribute( Qt::WA_LayoutUsesWidgetRect );

    this->setCursor( Qt::PointingHandCursor );
}

void
TagWidget::paintEvent( QPaintEvent* event )
{
    QPainter p( this );

    QFontMetrics fm = fontMetrics();

    QRect middleRect = rect();
    middleRect.adjust( m_left.width(), 0, 0, 0 );
    middleRect.setWidth( fm.width( text() ) + 8 );

    p.drawPixmap( rect().topLeft(), m_left );
    p.drawPixmap( middleRect, m_middle );
    p.drawPixmap( QPoint( m_left.width() + middleRect.width(), 0 ), m_right );

    QTextOption to;
    to.setAlignment( Qt::AlignCenter );

    p.drawText( middleRect.adjusted( 0, 0, 0, -1 ), text(), to );
}

QSize
TagWidget::sizeHint() const
{
    QFontMetrics fm = fontMetrics();
    QSize size = QPushButton::sizeHint();
    size.setWidth( m_left.width() + m_right.width() + fm.width( text() ) + 8 );
    size.setHeight( 19 );
    return size;
}

void
TagWidget::onClicked()
{
    QDesktopServices::openUrl( m_url );
}

const QPixmap*
TagWidget::left() const
{
    return &m_left;
}

void
TagWidget::setLeft( const QPixmap& left )
{
    m_left = left;
    update();
}

const QPixmap*
TagWidget::middle() const
{
    return &m_middle;
}

void
TagWidget::setMiddle( const QPixmap& middle )
{
    m_middle = middle;
    update();
}

const QPixmap*
TagWidget::right() const
{
    return &m_right;
}

void
TagWidget::setRight( const QPixmap& right )
{
    m_right = right;
    update();
}
