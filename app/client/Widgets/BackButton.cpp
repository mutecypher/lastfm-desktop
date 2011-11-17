
#include <QPainter>
#include <QFont>

#include "BackButton.h"

BackButton::BackButton(QWidget *parent) :
    QPushButton(parent)
{
}

void
BackButton::setDescription( const QString& description )
{
    m_description = description;
}

void
BackButton::paintEvent( QPaintEvent* e )
{
    QPushButton::paintEvent( e );

    QPainter p( this );

    QFont font = p.font();
    font.setWeight( QFont::Bold );
    font.setPixelSize( 14 );
    p.setFont( font );

    QTextOption to;
    to.setAlignment( Qt::AlignVCenter | Qt::AlignLeft );
    QRect descRect = rect().adjusted( 50, 16, -20, 0 );
    QFontMetrics fm( font );
    p.drawText( descRect, fm.elidedText( m_description, Qt::ElideRight, descRect.width() ), to );
}

